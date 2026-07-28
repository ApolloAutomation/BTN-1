# Upstream issue drafts for luar123/zigbee_esphome

All findings verified against branch `v1.x` at commit `6e3d549` ("require platformio for basic mode"). Line numbers below refer to that commit. Issues 1 through 6 are ready to file as-is; the last section covers feedback better suited to a discussion thread.

---

## Issue 1: Pending report can transmit a stale value when it wins the lock race against a pending set

**Title:** ZigBeeAttribute::loop() can report a stale attribute value when set_attr_() loses the Zigbee lock race

**Affected:** branch `v1.x`, commit `6e3d549` (logic present in all recent v1.x revisions)

**Code:** `components/zigbee/zigbee_attribute.cpp`

`ZigBeeAttribute::loop()` (lines 82-94) services a pending set, then a pending report:

```cpp
void ZigBeeAttribute::loop() {
  if (this->set_attr_requested_) {
    this->set_attr_();
  }

  if (this->report_requested_) {
    this->report_();
  }
  ...
}
```

Both operations independently try to take the Zigbee stack lock with a 20 ms timeout: `set_attr_()` at line 10 (`esp_zb_lock_acquire(20 / portTICK_PERIOD_MS)`) and `report_()` at line 32. Neither failure path defers the other. If `set_attr_()` times out (for example because the stack task holds the lock during a poll or an incoming frame) but the stack releases the lock a moment later, `report_()` acquires it successfully in the same loop pass. The report command then reads the cluster's attribute storage, which still holds the previous value because `esp_zb_zcl_set_attribute_val` never ran, and `report_()` clears `report_requested_` at line 46. On the next loop pass `set_attr_()` succeeds and applies the new value, but no report is pending anymore, so the new value is never transmitted.

**Failure scenario:** We hit this on real hardware with a button mapped to a binary attribute. The press edge set and reported correctly. On the release edge, `set_attr_()` lost its lock acquire and `report_()` won its own, so the release report went on the air carrying the still-pressed value. The coordinator showed the button stuck on until the next unrelated report.

**Suggested fix:** One line in `loop()`: do not run a report while a set is still pending.

```cpp
  if (this->report_requested_ && !this->set_attr_requested_) {
    this->report_();
  }
```

The report simply waits one or more loop passes until the set has applied, at which point it reports the correct value. Note that the `force_report_` path is already immune: `set_attr_()` calls `report_(true)` at lines 13-15 inside the same lock hold, so the set and report are atomic there. This change only affects the plain `report_enabled` path.

---

## Issue 2: find_attr() raises KeyError when a config mixes zigbee.setAttr with id-less attributes

**Title:** find_attr() crashes with KeyError instead of resolving zigbee.setAttr target when earlier attributes have no id

**Affected:** branch `v1.x`, commit `6e3d549`

**Code:** `components/zigbee/__init__.py`, lines 443-449:

```python
def find_attr(conf, id):
    for ep in conf[CONF_ENDPOINTS]:
        for cl in ep.get(CONF_CLUSTERS, []):
            for attr in cl.get(CONF_ATTRIBUTES, []):
                if attr[CONF_ID] == id:
                    return attr
    raise EsphomeError(f"Zigbee: Cannot find attribute {id}.")
```

Line 447 indexes `attr[CONF_ID]` directly. Attributes without an id are legal and common: validation only injects one when `device`, `on_value`, or `on_report` is present (lines 250-253), and `attributes_to_code` explicitly handles the id-less case with `attr.get(CONF_ID) is None` (line 454). `find_attr` is called from `zigbee_set_attr_to_code` (lines 704-707) to resolve the target of a `zigbee.setAttr` action.

**Failure scenario:** Any configuration that uses `zigbee.setAttr` and declares at least one id-less attribute earlier in declaration order than the target fails at code generation with a raw `KeyError: 'id'` traceback rather than a config error or a successful build. The workaround is to reorder attributes or give every attribute an id, which is not obvious from the error.

**Suggested fix:** Use the same tolerant lookup as the rest of the file:

```python
                if attr.get(CONF_ID) == id:
```

---

## Issue 3: ZigBeeComponent::reset() can hang the main loop until the task watchdog reboots

**Title:** reset() acquires the Zigbee lock with portMAX_DELAY on the ESPHome main thread

**Affected:** branch `v1.x`, commit `6e3d549`

**Code:** `components/zigbee/zigbee.h`, lines 98-102:

```cpp
  void reset() {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_factory_reset();
    esp_zb_lock_release();
  }
```

`reset()` is invoked from `ResetZigbeeAction` (`components/zigbee/automation.h`, lines 19-24), which runs on the ESPHome main thread.

**Failure scenario:** If the Zigbee stack task is wedged or holds the lock indefinitely (radio fault, stack assert, deadlocked callback), `esp_zb_lock_acquire(portMAX_DELAY)` never returns. The ESPHome main loop stops, so API, WiFi, and all other components stall, and the task watchdog eventually reboots the device. A factory reset triggered in the field, often as a last-resort recovery step in exactly the situations where the stack is misbehaving, turns into a watchdog reboot without the reset ever executing. This is also inconsistent with the rest of the component, which uses bounded 20 ms acquires everywhere else.

**Suggested fix:** Use a bounded acquire and retry from `loop()`, mirroring the `set_attr_requested_` pattern in `ZigBeeAttribute`: set a `reset_requested_` flag, attempt `esp_zb_lock_acquire(20 / portTICK_PERIOD_MS)` each pass, and log an error if it persistently fails so the user learns the stack is unresponsive instead of hitting a watchdog reset.

---

## Issue 4: CHAR_STRING attributes padded to max_size ship uninitialized heap bytes on the air

**Title:** get_zcl_string() with use_max_size copies only strlen bytes into a max_size buffer, leaking uninitialized heap memory

**Affected:** branch `v1.x`, commit `6e3d549`

**Code:** `components/zigbee/zigbee.cpp`, lines 42-51:

```cpp
uint8_t *get_zcl_string(const char *str, uint8_t max_size, bool use_max_size) {
  uint8_t str_len = static_cast<uint8_t>(strlen(str));
  uint8_t zcl_str_size = use_max_size ? max_size : std::min(max_size, str_len);

  uint8_t *zcl_str = new uint8_t[zcl_str_size + 1];  // string + length octet
  zcl_str[0] = zcl_str_size;
  memcpy(zcl_str + 1, str, str_len);

  return zcl_str;
}
```

With `use_max_size == true`, the buffer is allocated at `max_size + 1` bytes and the ZCL length octet is set to `max_size`, but only `str_len` bytes are ever written. `new uint8_t[n]` does not zero-initialize, so bytes `[str_len, max_size)` are whatever was previously on the heap. This path is used by `ZigBeeComponent::add_attr` (`components/zigbee/zigbee.h`, lines 171-178), which deliberately pads the initial value to `max_size` so the SDK sizes the attribute storage for later longer values.

**Failure scenario:** Declare a string attribute with `max_length` larger than the initial value literal. The attribute registered with the stack claims `max_size` bytes of content, and every read response or report for that attribute transmits the uninitialized tail over the air until the attribute is first rewritten at runtime. That is both garbage trailing content in the string and a small information leak of heap contents.

**Suggested fix:** Zero-fill the padding, either by value-initializing the allocation (`new uint8_t[zcl_str_size + 1]()`) or with an explicit `memset(zcl_str + 1 + str_len, 0, zcl_str_size - str_len)` when `zcl_str_size > str_len`. Alternatively set the length octet to `str_len` while still allocating `max_size + 1`, if the SDK sizes storage from the allocation rather than the length octet.

Related observation in the same function: when `use_max_size == false` and `str_len > max_size`, `zcl_str_size` truncates to `max_size` but the `memcpy` still copies `str_len` bytes, overflowing the allocation. `strncpy` style bounding of the copy length would close that too.

---

## Issue 5: Partition table validation accepts a table containing only one of zb_storage / zb_fct

**Title:** Custom partition table check uses boolean and instead of or, so a table missing one Zigbee partition passes validation

**Affected:** branch `v1.x`, commit `6e3d549`

**Code:** `components/zigbee/__init__.py`, lines 275-283:

```python
    if CONF_PARTITIONS in esp_conf:
        with open(
            CORE.relative_config_path(esp_conf[CONF_PARTITIONS]), encoding="utf8"
        ) as f:
            partitions = f.read()
            if ("zb_storage" not in partitions) and ("zb_fct" not in partitions):
                raise cv.Invalid(
                    "Add \n'zb_storage, data, fat,   , 16K,'\n'zb_fct, data, fat, , 1K,'\n to your custom partition table."
                )
```

The condition on line 280 only raises when both partitions are missing. A custom table that defines `zb_fct` but not `zb_storage` (or the reverse) sails through config validation.

**Failure scenario:** The build succeeds, the device flashes, and the Zigbee stack then fails at boot with a runtime error such as "Failed to find zb_storage partition" instead of the clear build-time message this check was written to provide. Users adding a custom partition table for OTA or larger app slots commonly copy only one of the two lines from an example.

**Suggested fix:** Require both partitions:

```python
            if ("zb_storage" not in partitions) or ("zb_fct" not in partitions):
```

Optionally split into two checks so the message names the specific missing partition.

---

## Issue 6: Operator precedence bug in time sync status check disables clock updates from Synchronized-only coordinators

**Title:** sync_status & 0x3 != 0 parses as sync_status & (0x3 != 0), testing only the Master bit

**Affected:** branch `v1.x`, commit `6e3d549`

**Code:** `components/zigbee/time/zigbee_time.cpp`, line 49:

```cpp
  if ((utc != 0) && (sync_status & 0x3 != 0)) { /* 0x3 = either Master or Syncronized bits set */
```

In C++, `!=` binds tighter than `&`, so this evaluates as `sync_status & (0x3 != 0)`, which is `sync_status & 1`. The comment states the intent: accept either the Master bit (0x1) or the Synchronized bit (0x2) of the Time cluster's TimeStatus attribute. As written, only the Master bit is tested.

**Failure scenario:** A coordinator that advertises only the Synchronized bit in TimeStatus (a legal and common configuration for a coordinator that is itself synced from an upstream source rather than acting as time master) returns `sync_status == 0x2`. The check evaluates to `0x2 & 1 == 0`, so `set_utc_time()` at line 50 is never called and the device logs "Did not recieve both time and status; clock NOT updated" (line 52) on every sync attempt. The device clock never sets despite the coordinator providing valid UTC time.

**Suggested fix:**

```cpp
  if ((utc != 0) && ((sync_status & 0x3) != 0)) {
```

---

## Positive reports worth filing as discussion feedback

Two items that are feedback rather than bug reports, better suited to a GitHub discussion thread:

**Sleepy end device works on ESP32-C6.** `sleepy: true` with `keep_alive` runs reliably on ESP32-C6 hardware in combination with the ESPHome PR 12325 power management work. One practical constraint worth documenting for other users: `keep_alive` (the poll interval) must stay well under the parent's indirect transaction persistence time, about 7.7 seconds per the Zigbee spec and tighter on some coordinator implementations. If the device polls slower than the parent buffers indirect transactions, commands and queued frames addressed to the sleeping device are silently dropped. A note in the sleepy documentation, or a validation warning when `keep_alive` exceeds a few seconds, would save people some debugging.

**Minor leak in bindingTableCb.** `components/zigbee/zigbee.cpp` line 183 allocates a `zb_device_params_t` per binding table record with `calloc`, uses it for logging in the loop at lines 179-198, and never frees or stores it. The `req` structure is freed correctly (lines 173 and 212), but each record's device struct leaks. It is a few dozen bytes per binding per rejoin, so low priority, but a `free(device)` at the end of the loop body (or a stack allocation, since nothing retains the pointer) would tidy it up.

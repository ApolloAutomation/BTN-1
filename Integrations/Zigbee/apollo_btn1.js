/**
 * Zigbee2MQTT External Converter for Apollo BTN-1
 *
 * This converter supports the Apollo BTN-1 4-button Zigbee remote
 * built with the zigbee_esphome component.
 *
 * Installation:
 * 1. Copy this file to your Zigbee2MQTT external_converters directory
 *    (typically: /config/zigbee2mqtt/ or where your configuration.yaml is)
 * 2. Add to your Zigbee2MQTT configuration.yaml:
 *    external_converters:
 *      - apollo_btn1.js
 * 3. Restart Zigbee2MQTT
 * 4. Remove and re-pair your BTN-1 device
 *
 * Debugging:
 * If entities don't appear correctly, check the device interview in Z2M:
 * - Go to Z2M frontend → Device → Dev console → Read clusters
 * - Note the actual endpoint numbers and adjust the endpoint() function below
 *
 * Device features:
 * - 4 buttons (binary input clusters)
 * - 4 RGB LED backlights (color light clusters)
 * - Battery level and voltage (power configuration cluster)
 * - Prevent sleep switch
 */

const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const utils = require('zigbee-herdsman-converters/lib/utils');
const e = exposes.presets;
const ea = exposes.access;

// Custom fromZigbee converter for binary input (button presses)
const fzLocal = {
    binary_input: {
        cluster: 'genBinaryInput',
        type: ['attributeReport', 'readResponse'],
        convert: (model, msg, publish, options, meta) => {
            const endpoint = msg.endpoint.ID;
            const value = msg.data['presentValue'];

            // Map endpoint to button number (adjust based on actual device)
            let buttonNum = endpoint;
            if (endpoint >= 1 && endpoint <= 4) {
                buttonNum = endpoint;
            }

            const action = value ? `button_${buttonNum}_press` : `button_${buttonNum}_release`;
            return {action};
        },
    },
};

const definition = {
    zigbeeModel: ['BTN-1'],
    model: 'BTN-1',
    vendor: 'Apollo Automation',
    description: 'Apollo BTN-1 4-button Zigbee remote with RGB backlighting and battery',

    fromZigbee: [
        fzLocal.binary_input,
        fz.on_off,
        fz.brightness,
        fz.color_colortemp,
        fz.battery,
    ],

    toZigbee: [
        tz.on_off,
        tz.light_onoff_brightness,
        tz.light_color,
        tz.light_color_colortemp,
    ],

    exposes: [
        // Button actions
        e.action([
            'button_1_press', 'button_1_release',
            'button_2_press', 'button_2_release',
            'button_3_press', 'button_3_release',
            'button_4_press', 'button_4_release',
        ]),

        // Battery
        e.battery(),
        e.battery_voltage(),

        // RGB Lights (one per button)
        // Endpoint names should match what the device reports
        e.light_brightness_color({colorTempRange: undefined}).withEndpoint('btn_1_light'),
        e.light_brightness_color({colorTempRange: undefined}).withEndpoint('btn_2_light'),
        e.light_brightness_color({colorTempRange: undefined}).withEndpoint('btn_3_light'),
        e.light_brightness_color({colorTempRange: undefined}).withEndpoint('btn_4_light'),

        // Prevent sleep switch
        e.switch().withEndpoint('prevent_sleep'),
    ],

    endpoint: (device) => {
        // Endpoint mapping - zigbee_esphome assigns endpoints sequentially
        // Based on Core.yaml component order:
        // 1-4: Buttons (binary_sensor)
        // 5-6: Battery sensors
        // 7: Prevent Sleep switch
        // 8-11: Lights (btn_1_light through btn_4_light)
        //
        // NOTE: These numbers may need adjustment!
        // Check device interview in Z2M Dev Console to verify actual endpoints.
        return {
            'button_1': 1,
            'button_2': 2,
            'button_3': 3,
            'button_4': 4,
            'battery': 5,
            'prevent_sleep': 7,
            'btn_1_light': 8,
            'btn_2_light': 9,
            'btn_3_light': 10,
            'btn_4_light': 11,
        };
    },

    meta: {
        multiEndpoint: true,
        multiEndpointSkip: ['battery', 'battery_voltage'],
    },

    configure: async (device, coordinatorEndpoint, logger) => {
        // Iterate through all endpoints and configure appropriate clusters
        for (const endpoint of device.endpoints) {
            const epId = endpoint.ID;

            try {
                // Battery reporting (genPowerCfg cluster)
                if (endpoint.supportsInputCluster('genPowerCfg')) {
                    await reporting.bind(endpoint, coordinatorEndpoint, ['genPowerCfg']);
                    await reporting.batteryPercentageRemaining(endpoint);
                    await reporting.batteryVoltage(endpoint);
                    logger.info(`BTN-1: Configured battery reporting on endpoint ${epId}`);
                }

                // Binary input for buttons (genBinaryInput cluster)
                if (endpoint.supportsInputCluster('genBinaryInput')) {
                    await reporting.bind(endpoint, coordinatorEndpoint, ['genBinaryInput']);
                    logger.info(`BTN-1: Configured binary input on endpoint ${epId}`);
                }

                // On/Off for switches
                if (endpoint.supportsInputCluster('genOnOff')) {
                    await reporting.bind(endpoint, coordinatorEndpoint, ['genOnOff']);
                    await reporting.onOff(endpoint);
                    logger.info(`BTN-1: Configured on/off on endpoint ${epId}`);
                }

                // Light clusters
                if (endpoint.supportsInputCluster('lightingColorCtrl')) {
                    await reporting.bind(endpoint, coordinatorEndpoint, [
                        'genOnOff',
                        'genLevelCtrl',
                        'lightingColorCtrl'
                    ]);
                    logger.info(`BTN-1: Configured light on endpoint ${epId}`);
                }
            } catch (e) {
                logger.warn(`BTN-1: Failed to configure endpoint ${epId}: ${e.message}`);
            }
        }
    },
};

module.exports = definition;

/**
 * Zigbee2MQTT external converter for the Apollo Automation BTN-1
 * (Zigbee firmware model BTN-1-Z2M, firmware 26.7.19.1-zigbee or newer).
 *
 * Installation on Zigbee2MQTT 2.x:
 * 1. Copy this file into the external_converters folder inside your
 *    Zigbee2MQTT data directory (create the folder if it does not exist).
 * 2. Restart Zigbee2MQTT.
 * 3. Pair the BTN-1, or re-interview it if it was already paired.
 *
 * On Zigbee2MQTT 1.x, additionally list the file under external_converters
 * in configuration.yaml.
 *
 * Endpoint map (fixed by the firmware):
 *   1-4: Buttons 1-4. genBinaryInput presentValue = raw pressed state.
 *        genMultistateInput presentValue = action event:
 *        1 single, 2 double, 3 triple, 4 hold.
 *        Endpoint 1 also carries genPowerCfg (battery).
 *   5-8: BTN 1-4 Lights. genOnOff, genLevelCtrl, lightingColorCtrl (XY only).
 */

const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const {light} = require('zigbee-herdsman-converters/lib/modernExtend');
const e = exposes.presets;
const ea = exposes.access;

const BUTTON_ENDPOINTS = [1, 2, 3, 4];
const LIGHT_ENDPOINTS = [5, 6, 7, 8];
const ACTION_NAMES = {1: 'single', 2: 'double', 3: 'triple', 4: 'hold'};

const fzLocal = {
    // Raw pressed state from genBinaryInput on endpoints 1-4
    button_state: {
        cluster: 'genBinaryInput',
        type: ['attributeReport', 'readResponse'],
        convert: (model, msg, publish, options, meta) => {
            const ep = msg.endpoint.ID;
            if (!BUTTON_ENDPOINTS.includes(ep)) return;
            if (msg.data.presentValue === undefined) return;
            return {[`button_${ep}_pressed`]: msg.data.presentValue === 1 || msg.data.presentValue === true};
        },
    },
    // Action events from genMultistateInput on endpoints 1-4
    button_action: {
        cluster: 'genMultistateInput',
        type: ['attributeReport'],
        convert: (model, msg, publish, options, meta) => {
            const ep = msg.endpoint.ID;
            if (!BUTTON_ENDPOINTS.includes(ep)) return;
            const name = ACTION_NAMES[msg.data.presentValue];
            if (!name) return;
            return {action: `button_${ep}_${name}`};
        },
    },
};

const definition = {
    zigbeeModel: ['BTN-1-Z2M'],
    model: 'BTN-1',
    vendor: 'Apollo Automation',
    description: 'Four button Zigbee macro deck with RGB backlighting and battery monitoring',

    extend: [
        light({
            color: {modes: ['xy']},
            endpointNames: ['l1', 'l2', 'l3', 'l4'],
        }),
    ],

    fromZigbee: [fzLocal.button_state, fzLocal.button_action, fz.battery],
    toZigbee: [],

    exposes: [
        e.action([
            'button_1_single', 'button_1_double', 'button_1_triple', 'button_1_hold',
            'button_2_single', 'button_2_double', 'button_2_triple', 'button_2_hold',
            'button_3_single', 'button_3_double', 'button_3_triple', 'button_3_hold',
            'button_4_single', 'button_4_double', 'button_4_triple', 'button_4_hold',
        ]),
        e.binary('button_1_pressed', ea.STATE, true, false)
            .withDescription('Button 1 raw pressed state'),
        e.binary('button_2_pressed', ea.STATE, true, false)
            .withDescription('Button 2 raw pressed state'),
        e.binary('button_3_pressed', ea.STATE, true, false)
            .withDescription('Button 3 raw pressed state'),
        e.binary('button_4_pressed', ea.STATE, true, false)
            .withDescription('Button 4 raw pressed state'),
        e.battery(),
        e.battery_voltage(),
    ],

    endpoint: (device) => {
        return {
            'l1': 5, 'l2': 6, 'l3': 7, 'l4': 8,
        };
    },

    meta: {
        multiEndpoint: true,
        multiEndpointSkip: ['battery', 'voltage', 'action'],
    },

    configure: async (device, coordinatorEndpoint) => {
        for (const epId of BUTTON_ENDPOINTS) {
            const endpoint = device.getEndpoint(epId);
            if (!endpoint) continue;
            await reporting.bind(endpoint, coordinatorEndpoint, ['genBinaryInput', 'genMultistateInput']);
        }
        const ep1 = device.getEndpoint(1);
        if (ep1 && ep1.supportsInputCluster('genPowerCfg')) {
            await reporting.bind(ep1, coordinatorEndpoint, ['genPowerCfg']);
            await reporting.batteryPercentageRemaining(ep1);
            await reporting.batteryVoltage(ep1);
        }
        for (const epId of LIGHT_ENDPOINTS) {
            const endpoint = device.getEndpoint(epId);
            if (!endpoint) continue;
            await reporting.bind(endpoint, coordinatorEndpoint, ['genOnOff', 'genLevelCtrl', 'lightingColorCtrl']);
        }
    },
};

module.exports = definition;

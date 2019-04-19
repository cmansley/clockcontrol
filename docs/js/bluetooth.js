async function connect() {
    let filters = [];
    let serviceid = parseInt('0xFFE0');
    filters.push({ services: [serviceid] });
    let options = {};
    options.filters = filters
    return navigator.bluetooth.requestDevice(options).
        then(device => {
            console.log('"' + device.name + '" bluetooth device selected');
            return device.gatt.connect().
                then((server) => {
                    console.log('GATT server connected', 'Getting service...');
                    return server.getPrimaryService(serviceid);
                }).
                then((service) => {
                    console.log('Service found', 'Getting characteristic...');
                    return service.getCharacteristic(parseInt('0xFFE1'));
                })
        });
}

async function write(characteristic, text) {
    return characteristic.writeValue(new TextEncoder().encode(text));
}

function hexToRGBClock(h) {
    let r = "0x" + h[1] + h[2];
    let g = "0x" + h[3] + h[4];
    let b = "0x" + h[5] + h[6];
    return "r" + parseInt(r).toString() + "g" + parseInt(g).toString() + "b" + parseInt(b).toString();
}
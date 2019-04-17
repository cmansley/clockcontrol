function onButtonClick() {
    let filters = [];
    let serviceid = parseInt('0xFFE0');
    filters.push({ services: [serviceid] });
    let options = {};
    options.filters = filters
    navigator.bluetooth.requestDevice(options).
        then(device => {
            console.log('"' + device.name + '" bluetooth device selected');
            device.gatt.connect().
                then(server => {
                    console.log('GATT server connected', 'Getting service...');
                    return server.getPrimaryService(serviceid);
                }).
                then(service => {
                    console.log('Service found', 'Getting characteristic...');
                    return service.getCharacteristic(parseInt('0xFFE1'));
                }).
                then(characteristic => {
                    console.log('Characteristic found');
                    characteristic.writeValue(new TextEncoder().encode('m'));
                });
        });
}
if exist('tx', 'var')
    tx.release();
end

plutoradiosetup();
configurePlutoRadio('AD9364')

tx = sdrtx('Pluto');
tx.RadioID = 'usb:0';
tx.CenterFrequency = 5899e6;
tx.BasebandSampleRate = 20e6;
tx.Gain = 0;

tx.ShowAdvancedProperties = true;
tx.FrequencyCorrection = 8.71;

F0 = 1/20;
x = exp(1i*2*pi*F0*(0:199)'); % 10 periods

tx.transmitRepeat(x);
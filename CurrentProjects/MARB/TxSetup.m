clear;
plutoradiosetup();
N = 128;
x = zeros(N,1);
for idx = -63:2:64
    x = x + exp(1i*2*pi/(N)*idx*(0:N-1)');
end
mxm = max(abs(x));
if mxm > 1
    x = x ./ mxm;
end

tx = sdrtx('Pluto');
tx.RadioID = 'usb:0';
tx.CenterFrequency = 1250e6; % 1.25 GHz
tx.BasebandSampleRate = 20e6; % 20 MegaSamples/s
tx.Gain = -13;
tx.ShowAdvancedProperties = true;
%  - (.3125e6 / 2)
transmitRepeat(tx,x);
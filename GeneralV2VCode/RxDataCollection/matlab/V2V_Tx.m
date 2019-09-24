if exist('tx', 'var')
    tx.release();
end

plutoradiosetup();
configurePlutoRadio('AD9364')

tx = sdrtx('Pluto');
tx.RadioID = 'usb:0';
tx.CenterFrequency = 5900e6;
tx.BasebandSampleRate = 20e6;
tx.Gain = 0;

tx.ShowAdvancedProperties = true;
tx.FrequencyCorrection = 5.33;

df = 0.3125; % subcarrier separation in MHz
BW = 20; % BW in MHz
Fs = 20; % sample rate in MHz
f0 = 70; % bandpass frequency in MHz

dF = df/Fs;
N0 = 1/dF;
F0 = f0/Fs;

load optimal_phases64.mat; % theta_opt variable

x = zeros(N0,1);
for idx = -31:32
    x = x + exp(1i*2*pi*dF*idx*(0:N0-1)') * exp(1i*theta_opt(idx + 32));
end

mxm = max(abs(x));
if mxm > 1
    x = x ./ mxm;
end

tx.transmitRepeat(x);
%%% V2VtxMexico

load soundingSignalBW10DF015625Fs40.mat % xl;
% load soundingSignalBW20DF015625Fs40.mat % xl;
Fs = 40;

plutoradiosetup();
configurePlutoRadio('AD9364')

tx = sdrtx('Pluto');
tx.RadioID = 'usb:0';
tx.CenterFrequency = 5900e6;
tx.BasebandSampleRate = Fs * 1e6;
tx.Gain = -13;

tx.ShowAdvancedProperties = true;
tx.FrequencyCorrection = 5.33;

tx.transmitRepeat(xl);
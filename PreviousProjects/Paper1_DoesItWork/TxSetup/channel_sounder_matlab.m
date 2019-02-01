%%% channel_sounder_matlab %%%

% This file is used to set up a ADALM-Pluto radio to transmit. After the
% file is finished running the transmitter will begin to transmit.
% Additionally, if there is an external power source for the radio, it may
% be disconnected from the computer and it will continue to transmit. 
% However, should the radio lose power at any time it will cease to 
% transmit and needs to be set up once more.

channel_sounder_src;
tx = sdrtx('Pluto');
tx.RadioID = 'usb:0';
tx.CenterFrequency = 1250e6;
tx.BasebandSampleRate = 20e6;
tx.Gain = -13;
tx.ShowAdvancedProperties = true;

transmitRepeat(tx,x);
% Use the info method to show the actual values of various hardware-related
% properties
% txRadioInfo = info(tx)

% release(tx);
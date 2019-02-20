%%% Setup %%%

% RUN THIS FILE ONCE EACH TIME THE PLUTO RADIO IS FIRST CONNECTED TO YOUR
% COMPUTER
% Obs: the command rx_1.release if a radio is disconnected from your
% computer and then reconnected and you desire to rerun setup without
% restarting matlab.

plutoradiosetup();
rx_1 = sdrrx('Pluto');
rx_1.RadioID = 'usb:0';
rx_1.CenterFrequency = 1250e6;
rx_1.BasebandSampleRate = 20e6;
rx_1.SamplesPerFrame = 32*64;
rx_1.OutputDataType = 'double';
rx_1.ShowAdvancedProperties = true;

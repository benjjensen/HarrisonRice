%%% Setup %%%

% RUN THIS FILE ONCE EACH TIME THE PLUTO RADIO IS FIRST CONNECTED TO YOUR
% COMPUTER
% Obs: the command rx.release should be used if a radio is disconnected 
% from your computer and then reconnected and you desire to rerun setup 
% without restarting matlab.

plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

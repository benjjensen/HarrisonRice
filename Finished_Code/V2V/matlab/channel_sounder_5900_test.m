%%% channel_sounder_5900_test

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% (1) Connect the Pluto USB to the computer
%%% (2) Connect the Tx port to the spectrum analyzer using a cable
%%% (3) Setup the spectrum analyzer as follows:
%%%     Start Freq = 5880 MHz
%%%     Stop Freq = 5920 MHz
%%% (4) Run this script
%%%     "configurePlutoRadio('AD9364')" takes a few minutes
%%% (5) Take a picture of the Spectrum Analyzer screen.
%%% (6) At the Matlab command prompt, type tx.release
%%% (7) Get a different Pluto radio, go to (1).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

plutoradiosetup();
configurePlutoRadio('AD9364')

tx = sdrtx('Pluto');
tx.RadioID = 'usb:0';
tx.CenterFrequency = 5900e6;
tx.BasebandSampleRate = 20e6;
tx.Gain = -20;
tx.ShowAdvancedProperties = true;

% x = channel_sounder_src;
channel_sounder_src;

tx.transmitRepeat(x); % this is the object oriented way

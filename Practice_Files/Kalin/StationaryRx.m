%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Setup();

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Data Collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
starttime = '01-Aug-2018 10:36:40';
curtime = datetime;
while (string(curtime) ~= string(starttime))
    curtime = datetime;
    disp(string(curtime));
end

NumSamples = 5000;
DelayTime = .5;
TotalRunTime = (NumSamples * DelayTime) / 60;
disp('This test will last for ' + string(TotalRunTime) + ' minutes');

for runs = 1:NumSamples
    
    











function Setup()
plutoradiosetup();
rx_1 = sdrrx('Pluto');
rx_1.RadioID = 'usb:0';
rx_1.CenterFrequency = 1250e6;
rx_1.BasebandSampleRate = 20e6;
rx_1.SamplesPerFrame = 32*64;
rx_1.OutputDataType = 'double';
rx_1.ShowAdvancedProperties = true;
end
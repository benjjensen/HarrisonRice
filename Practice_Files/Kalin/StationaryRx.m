%%%% StationaryRx %%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Setup for the first run
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% rx = Setup();

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Data Collection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

starttime = datetime('now') + seconds(.5);
curtime = datetime;
while (string(curtime) ~= string(starttime))
    curtime = datetime;
    disp(string(curtime));
end
tic;
room = "testing";
NumSamples = 30;
DelayTime = .5 ...
    - .02268;
TotalRunTime = (NumSamples * (DelayTime + .02268)) / 60;
disp('This test will last for ' + string(TotalRunTime) + ' minutes');

for runs = 1:NumSamples
    time = datetime;
    time = char(time);
    time = strrep(time,' ','-');
    time = string(time);
    eval(sprintf('%s_%d = rx();',room,runs));
    eval(sprintf('save("StationaryData/%s_%d.mat","%s_%d");',room,runs,room,runs));
    pause(DelayTime);
end

toc;









function [rx] = Setup()
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;
end
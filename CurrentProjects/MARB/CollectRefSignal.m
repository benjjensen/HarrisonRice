clear; close all;
warning('off','all');

%%% User Inputs
refSignalName = 'Radio1_ref';
%%%%%%%%%%%%%%%

%% Radio Setup
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

%% Flush Buffer
% While this most likely isn't necessary, it's included to be safe
for ignoreme = 1:50
    dostuff = rx();
end

%% Collect Reference Signal
refSignal = zeros(2048,500);

for frame = 1:500
    refSignal(:,frame) = rx();
end

%% Save Reference Signal
save([refSignalName '.mat'], 'refSignal');

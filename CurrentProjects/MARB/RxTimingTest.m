clear; close all;
warning('off','all'); % Disables warnings

%% Radio Setup
tic

plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

for ignoreme = 1:50
    dostuff = rx();
end

toc
disp('The time listed above is the radio setup time');
disp(' ');

%% Basic Timing Test
frames = 500;
disp('Timing test for ' + string(frames) + ' frames');
pauseTime = 0;
fillMe = zeros(frames,2048);
for getNumerousTimeVals = 1:10
    tic

    for radioRuns = 1:frames
        fillMe(radioRuns,:) = rx();
        pause(pauseTime);
    end
    
    toc
end

% Radio 1 - Dr Harrison
% Radio 2 - Farah - Setup Time: 19.26 seconds, Run Time Max: 2.54 seconds
% Radio 3 - Kalin
% Radio 4 - Benj
% Radio 5 - ?
% Radio 6 - ?
% Radio 7 - Autumn (Work Laptop)
% Radio 8 - ?
% Radio 9 - Bryan
% Radio 10 - Tx (do not need to test)
% Radio 11 - Spencer
% Radio 12 - Morteza

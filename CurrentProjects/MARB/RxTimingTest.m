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

% Radio 1 - Dr Harrison - Setup Time: 35.5 seconds, Run Time Max: 1.9 seconds
% Radio 2 - Farah - Setup Time: 19.26 seconds, Run Time Max: 2.54 seconds
% Radio 3 - Kalin - Setup Time: 57.43 secdonds, Run Time Max: 3.39 seconds
% Radio 4 - Benj - Setup Time: 6-35 seconds, Run Time Max: 1.3-2.4 seconds
% Radio 5 - Amy (Harrison's Spare) - Setup Time: 64.5 seconds, Run Time Max: 1.7 seconds
% Radio 6 - Nathan?
% Radio 7 - Autumn (Work Laptop) - Setup Time: 25 , Run Time Max: 2-2.7 seconds
% Radio 8 - ?
% Radio 9 - Bryan - Setup Time: 32 seconds, Run Time Max: 2.7 seconds
% Radio 10 - Tx (do not need to test)
% Radio 11 - Spencer - Setup Time: 23.3 seconds, Run Time Max: 2.2 seconds
% Radio 12 - Morteza - Setup Time: 27.62 seconds, Run Time Max: 2.1 seconds

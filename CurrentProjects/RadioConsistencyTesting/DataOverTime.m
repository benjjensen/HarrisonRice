<<<<<<< HEAD
clearvars -except rx; close all;
% programstart = datetime;
% waitduration = duration('00:00:18');
warning('off','all')
%% Radio Setup
if (exist('rx','var') ~= 1)
    plutoradiosetup();
    rx = sdrrx('Pluto');
    rx.RadioID = 'usb:0';
    rx.CenterFrequency = 1250e6;
    rx.BasebandSampleRate = 20e6;
    rx.SamplesPerFrame = 32*64;
    rx.OutputDataType = 'double';
    rx.ShowAdvancedProperties = true;
end
%Data Collection
prompt = 'Which radio are you using? Please type ''Radio#'''; %This will become a file name to keep  track of our radio data
Radio = input(prompt,'s'); %prompts user to type, stored as a string
while isempty(Radio) %If you don't type anything but hit enter, you will be prompted again
    Radio = input(prompt, 's');
end
frames = 100; %uses less frames to take less time
test = zeros(2048,frames);

pause('on') %This is just to make sure pause is enabled
for i = 1:8
    for clearbuffer = 1:7
        buffer(:,1) = rx();
    end
    for runs = 1:frames
        test(:,runs) = rx();
    end
    save(['Tests/' Radio '_test_' num2str(i) '.mat'], 'test');
    % This is a very useful way to save multiple files in a loop. This will
    % produce files like Radio#test1,2,3,4, etc. Simple substitute for
    % sprintf and eval!
    
    pause(300); %pauses for 5 minutes 
end
=======
clearvars -except rx; close all;
% programstart = datetime;
% waitduration = duration('00:00:18');
warning('off','all')
%% Radio Setup
if (exist('rx','var') ~= 1)
    plutoradiosetup();
    rx = sdrrx('Pluto');
    rx.RadioID = 'usb:0';
    rx.CenterFrequency = 1250e6;
    rx.BasebandSampleRate = 20e6;
    rx.SamplesPerFrame = 32*64;
    rx.OutputDataType = 'double';
    rx.ShowAdvancedProperties = true;
end
%Data Collection
prompt = 'Which radio are you using? Please type ''Radio#'''; %This will become a file name to keep  track of our radio data
Radio = input(prompt,'s'); %prompts user to type, stored as a string
while isempty(Radio) %If you don't type anything but hit enter, you will be prompted again
    Radio = input(prompt, 's');
end
frames = 100; %uses less frames to take less time
test = zeros(2048,frames);

pause('on') %This is just to make sure pause is enabled
for i = 1:8
    for clearbuffer = 1:7
        buffer(:,1) = rx();
    end
    for runs = 1:frames
        test(:,runs) = rx();
    end
    save(['Tests/' Radio '_test_' num2str(i) '.mat'], 'test');
    % This is a very useful way to save multiple files in a loop. This will
    % produce files like Radio#test1,2,3,4, etc. Simple substitute for
    % sprintf and eval!
    
    pause(300); %pauses for 5 minutes 
end
>>>>>>> 0ac6576db0ea1276e8264f11a9dedfa5ff2aa3c9

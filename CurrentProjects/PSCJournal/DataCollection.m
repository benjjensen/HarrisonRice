%%%%%%% Rx Data Collection (code from previous project) %%%%%%%%
warning('OFF');
if (exist('rx','var'))
else
    plutoradiosetup();
    rx = sdrrx('Pluto');
    rx.RadioID = 'usb:0';
    rx.CenterFrequency = 1250e6;
    rx.BasebandSampleRate = 20e6;
    rx.SamplesPerFrame = 32*64;
    rx.OutputDataType = 'double';
    rx.ShowAdvancedProperties = true;
    
    % Burst Mode
    % rx.EnableBurstMode = true;
    % rx.NumFramesInBurst = 1;
    %%
end

timeDelay = 1; % delay between each sample
startDelay = 3; % delay before the first sample is taken
samples = 35; % number of locations
frames = zeros(2048,samples);
i = input('Enter the name of this run:\n','s'); % name of data array
eval([i,' = zeros(2048,samples);']); %% match name of this to previous line (change both instances of the name)

pause(startDelay);
% close all; % when graphing is also run, this closes all previous figures
for yAxis = 1:samples
    %     prompt = "press enter or type whatever you want and then press enter ";
    %     ManualWait = input(prompt,'s'); % we won't do anything with this variable, it is just to customize your time
    %     disp("Starting" + yAxis);
    for buffer = 1:7
        ignore = rx();
    end
    for framesperlocation = 1:50
        eval([i,'(:,yAxis,framesperlocation) = rx();']); % ultimately only one of the 8 frames will be kept
    end
    %     disp("Stopping" + yAxis);
    beep; % plays a sound on windows or apple system
    pause(timeDelay); % allows for time to move the receiver to the next location
end

% save('SPECIALFOLDER/'+string(i) + '.mat',string(i)); % Saves data array with custom name

%% After collecting all of the data in the room, uncomment the code below this line and run this section only
% name = input('Enter the name of the room in which you have been collecting data:\n','s');
% save(['SPECIALFOLDER/' name 'EmptyRaw.mat'], [name '*']);

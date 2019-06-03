%%%%%%% Rx Data Collection (code from previous project) %%%%%%%%
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

% Burst Mode
rx.EnableBurstMode = true;
rx.NumFramesInBurst = 1;

%%

timeDelay = 4; % delay between each sample
startDelay = 5; % delay before the first sample is taken
samples = 20; % number of locations
frames = zeros(2048,samples);
i = 'Testing1'; % name of data array
testing1 = zeros(2048,samples); %% match name of this to previous line (change both instances of the name)

pause(startDelay);
% close all; % when graphing is also run, this closes all previous figures
for yAxis = 1:samples
%     prompt = "press enter or type whatever you want and then press enter ";
%     ManualWait = input(prompt,'s'); % we won't do anything with this variable, it is just to customize your time
    disp("Starting" + yAxis);
    testing1(:,yAxis) = rx(); % ultimately only one of the 8 frames will be kept
    disp("Stopping" + yAxis);
    beep; % plays a sound on windows or apple system
    pause(timeDelay); % allows for time to move the receiver to the next location
end

save('SPECIALFOLDER/'+string(i) + '.mat',string(i)); % Saves data array with custom name

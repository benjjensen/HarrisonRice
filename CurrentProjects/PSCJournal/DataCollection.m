%%%%%%% Rx Data Collection (code from previous project) %%%%%%%%
%%% Setup for Sound on a Linux Machine (beep doesn't work on Linux)
res = 22050; 
len = 0.2 * res; 
hz = 400; 
%%%

timeDelay = 5; % delay between each sample
startDelay = 5; % delay before the first sample is taken
samples = 31; % number of locations
frames = zeros(2048,samples,8);
i = 'Room433_Column10'; % name of data array
Room433_Column10 = zeros(2048,samples); %% match name of this to previous line (change both instances of the name)

pause(startDelay);
% close all; % when graphing is also run, this closes all previous figures
for yAxis = 1:samples
    prompt = "press enter or type whatever you want and then press enter";
    ManualWait = input(prompt,'s'); % we won't do anything with this variable, it is just to customize your time
    disp("Starting" + yAxis);
    for consistent = 1:8 % This loop is to account for the built in buffer in the Pluto Radios
        frames(:,yAxis,consistent) = rx(); % ultimately only one of the 8 frames will be kept
    end % as for each location with our sample size, eight identical frames are captured
    disp("Stopping" + yAxis);
    beep; % plays a sound on windows or apple system
   % sound( sin( hz*(2*pi*(0:len)/res) ), res); % plays a sound on linux system
    pause(timeDelay); % allows for time to move the receiver to the next location
end
for transfer = 1:samples % This for loop goes through and keeps only one of the 8 frames
    Room433_Column10(:,transfer) = frames(:,transfer,8); % previously mentioned.
end
save('SPECIALFOLDER/'+string(i) + '.mat',string(i)); % Saves data array with custom name
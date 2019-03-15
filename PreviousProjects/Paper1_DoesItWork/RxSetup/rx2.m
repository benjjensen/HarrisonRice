
%%%%%%%%%%%%%%%%%%%%%%%%%% Rx Data Collection %%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Setup for Sound on a Linux Machine (beep doesn't work on Linux)
res = 22050; 
len = 0.2 * res; 
hz = 400; 
%%%

timeDelay = 0; % delay between each sample
startDelay = 0; % delay before the first sample is taken
samples = 10; % number of locations
frames = zeros(2048,samples,8);
i = 'test'; % name of data array
test = zeros(2048,samples); %% match name of this to previous line (change both instances of the name)

pause(startDelay);
% close all; % when graphing is also run, this closes all previous figures
for yAxis = 1:samples
    disp("Starting" + yAxis);
    for consistent = 1:8 % This loop is to account for the built in buffer in the Pluto Radios
        frames(:,yAxis,consistent) = rx_1(); % ultimately only one of the 8 frames will be kept
    end % as for each location with our sample size, eight identical frames are captured
    disp("Stopping" + yAxis);
%     beep; % plays a sound on windows or apple system
    sound( sin( hz*(2*pi*(0:len)/res) ), res); % plays a sound on linux system
    pause(timeDelay); % allows for time to move the receiver to the next location
end
for transfer = 1:samples % This for loop goes through and keeps only one of the 8 frames
    test(:,transfer) = frames(:,transfer,8); % previously mentioned.
end
save(string(i) + '.mat',string(i)); % Saves data array with custom name
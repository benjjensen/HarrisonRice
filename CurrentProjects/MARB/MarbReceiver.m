%%%%%%%%%%%% MARB Receiver %%%%%%%%%%%%%%
% Pulls in and saves data from an SDR
% (Sets up the radio, accounts for the buffer error)

clear; close all;
warning('off','all')   % Gets rid of the super annoying version warnings

%%% USER INFORMATION %%%%%%
frames = 1500; % approximately 1500 frames per minute, depending on the computer
nameOfArray = "Test";
%%%%%%%%%%%%%%%%%%%%%%%%%%%

    % Radio setup
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;


    % Account for the buffer
ignore = zeros(2048,1);
temp = zeros(2048, frames);
for init = 1:100
    ignore(:,1) = rx();
end

    % Gather Data
for count = 1:frames
    temp(:, count) = rx();
end 

    % Save the file
eval(sprintf("%s = temp;",string(nameOfArray)));
i = string(nameOfArray); % name of data array
save(string(i) + '.mat',string(i)); % Saves data array with custom name

beep;




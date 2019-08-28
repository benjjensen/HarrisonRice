%%%%%%%%%%%% MARB Receiver %%%%%%%%%%%%%%
% Pulls in and saves data from an SDR
% (Sets up the radio, accounts for the buffer error)

clear; close all;

%%% USER INFORMATION %%%%%%
frames = 8000; % approximately 8000 frames per minute, depending on the computer
nameOfArray = "harrisonTest";
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% warning('off','all')   % Gets rid of the super annoying version warnings

    %% Radio setup
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

    % Buffer
ignore = zeros(2048,1);
for init = 1:100
    ignore(:,1) = rx();
end

% framesPerMin = 8000;
% frames = minOfData * framesPerMin; % # of Min of Data

i = string(nameOfArray); % name of data array

temp = zeros(2048, frames);

% timePerSection = duration('00:00:03');
% numDivisions = (60*minOfData)/3; 
% framesPerRun = frames/numDivisions;
% split(numDivisions) = zeros;

% for init = 1:numDivisions
%     split(init) = init * framesPerRun;
% end

% for sections = 1:numDivisions
%     begintime = datetime;
%     for runs = split(sections)-(framesPerRun-1):split(sections)
%         temp(:,runs) = rx();
%     end
% %     while ((datetime - begintime) <= timePerSection)
% %     end
% end

eval(sprintf("%s = temp;",string(nameOfArray)));

save(string(i) + '.mat',string(i)); % Saves data array with custom name
beep;



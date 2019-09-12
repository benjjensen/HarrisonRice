%%%%%%%%%%%% MARB Receiver %%%%%%%%%%%%%%
% Pulls in and saves data from an SDR
% (Sets up the radio, accounts for the buffer error)

clearvars -except rx; close all;
warning('off','all')   % Gets rid of the super annoying version warnings

%%% USER INFORMATION %%%%%%
framesPerMinute = 7500; % approximately 4250 frames per minute, depending on the computer
framesPerSection = 500;
minutesOfData = 4;
pauseTime = 0.0;
nameOfArray = 'test';
%%%%%%%%%%%%%%%%%%%%%%%%%%%

DurationForRadioSetup = duration('00:0:05');
RadioSetupBeginTime = datetime;

if ~exist('rx','var')
        % Radio setup
    plutoradiosetup();
    rx = sdrrx('Pluto');
    rx.RadioID = 'usb:0';
    rx.CenterFrequency = 1250e6;
    rx.BasebandSampleRate = 20e6;
    rx.SamplesPerFrame = 32*64;
    rx.OutputDataType = 'double';
    rx.ShowAdvancedProperties = true;
end
    % Initial pull from the radio (helps sync timing)?
for ignoreme = 1:50
    dostuff = rx();
end

    % Initialize Variables
totalFrames = framesPerMinute * minutesOfData;
numSections = totalFrames / framesPerSection;
firstIndexOfSection = 1;
lastIndexOfSection = framesPerSection;
timePerSection = duration('00:0:04');
temp = zeros(2048, totalFrames);

    % Wait for the setup duration specified above
while((datetime - RadioSetupBeginTime) < DurationForRadioSetup)
end

    % Gather Data
DataCollectionStartTime = datetime;
for timingSections = 1:numSections
    SectionStartTime = datetime;
    for index = firstIndexOfSection:lastIndexOfSection
        temp(:,index) = rx();
        pause(pauseTime); % for the computers that run too fast...
    end
    firstIndexOfSection = firstIndexOfSection + framesPerSection;
    lastIndexOfSection = lastIndexOfSection + framesPerSection;
    while ((datetime - SectionStartTime) <= timePerSection)
    end
end
DataCollectionFinishTime = datetime;
DataCollectionRunTime = DataCollectionFinishTime - DataCollectionStartTime;

    % Save the file
eval(sprintf("%s = temp;",string(nameOfArray)));
i = string(nameOfArray); % name of data array
save(string(i) + '.mat',string(i)); % Saves data array with custom name

disp('Program Finished. Runtime was: ' + string(DataCollectionRunTime) + '. Results saved as ' + string(nameOfArray) + '.mat');

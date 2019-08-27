%%%%%%%%%%%% MARB Receiver %%%%%%%%%%%%%%
% Pulls in and saves data from an SDR
% (Sets up the radio, accounts for the buffer error)

clear; close all;

%%% USER INFORMATION %%%%%%
minOfData = 1;
nameOfArray = harrisonTest;
waitduration = duration('00:00:20');
%%%%%%%%%%%%%%%%%%%%%%%%%%%


    % Ensures that all computers start at similar times
programstart = datetime;
warning('off','all')

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

framesPerMin = 8000;
frames = minOfData * framesPerMin; % # of Min of Data

i = string(nameOfArray); % name of data array

eval(sprintf("%s = zeros(2048, frames);",string(nameOfArray)));

timePerSection = duration('00:00:03');
numDivisions = (60*minOfData)/3; 
framesPerRun = frames/numDivisions;
split(numDivisions) = zeros;
for init = 1:numDivisions
    split(init) = init * framesPerRun;
end

while((datetime - programstart) < waitduration)
    disp('waiting');
    pause(1);
end
starttime = datetime;
for sections = 1:numDivisions
    begintime = datetime;
    for runs = split(sections)-(framesPerRun-1):split(sections)
        eval(sprintf("%s(:,runs) = rx();",string(nameOfArray)));
    end
    while ((datetime - begintime) <= timePerSection)
    end
end
stoptime = datetime;
runtime = stoptime - starttime
save(string(i) + '.mat',string(i)); % Saves data array with custom name
beep;



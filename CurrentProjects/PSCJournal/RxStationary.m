clear;
programstart = datetime;
waitduration = duration('00:00:30');
warning('off','all')

plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;
    % Do we want to freeze the gain?
    rx.GainSource = 'Manual';
    rx.Gain = 62;

ignore = zeros(2048,1);
for init = 1:100
    ignore(:,1) = rx();
end

frames = 45000; % 5 Min of Data
buffer = zeros(2048,1); % Accounts for the buffer in the radios whenever a pause occurs between requests for data

i = 'test'; % name of data array
test = zeros(2048,frames); %% match name of this to previous line (change both instances of the name)

timePerSection = duration('00:00:03');
numDivisions = (60*5)/3; 
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
    for clearbuffer = 1:7
        buffer(:,1) = rx();
    end
    for runs = split(sections)-(framesPerRun-1):split(sections)
        test(:,runs) = rx();
        % pause(.001); % for the computers that run too fast...
    end
    while ((datetime - begintime) <= timePerSection)
    end
end
stoptime = datetime;
runtime = stoptime - starttime
% save(string(i) + '.mat',string(i)); % Saves data array with custom name
beep;

tic;
for checkduplicate = 1:frames-1
    if (test(:,checkduplicate) == test(:,checkduplicate+1))
        disp(string(checkduplicate) + 'is equal to ' + string(iterate));
    end
end
toc;


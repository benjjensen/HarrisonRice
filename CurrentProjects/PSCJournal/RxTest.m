clear;

plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

frames = 7000;
test = zeros(2048,frames);
ignore = zeros(2048,500);
for runs = 1:500
    ignore(:,runs) = rx();
end

test1begin = datetime;
for runs = 1:frames
    test(:,runs) = rx();
end
test1end = datetime;
test1time = test1end - test1begin

framesPerRun = 3500;
numDivisions = frames/framesPerRun;
split(numDivisions) = zeros;
for init = 1:numDivisions
    split(init) = init * framesPerRun;
end
test2 = zeros(2048,frames);
timePerSection = duration('00:00:30');

test2begin = datetime;
for sections = 1:numDivisions
    begintime = datetime;
    for runs = split(sections)-(framesPerRun-1):split(sections)
        test2(:,runs) = rx();
    end
    
    while ((datetime - begintime) <= timePerSection)
    end
end
test2end = datetime;
test2time = test2end - test2begin

for trials = 1:3
test3length = duration('00:00:25');
test3begin = datetime;
count = 0;
test3 = zeros(2048,1);
while ((datetime - test3begin) < test3length)
    test3(:,end+1) = rx();
    count = count + 1;
end
count
end
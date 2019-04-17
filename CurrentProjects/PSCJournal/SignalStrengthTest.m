clear; close all;
programstart = datetime;
waitduration = duration('00:00:18');
warning('off','all')
%% Radio Setup
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;
%% Data Collection
ignore = zeros(2048,1);
for init = 1:100
    ignore(:,1) = rx();
end
frames = 2000;
numDivisions = 5; 
framesPerRun = frames/numDivisions;
split(numDivisions) = zeros;
for init = 1:numDivisions
    split(init) = init * framesPerRun;
end
test = zeros(2048,frames);
buffer = zeros(2048,1);
timePerSection = duration('00:00:03');
while((datetime - programstart) < waitduration)
    disp('waiting');
    pause(1);
end

for sections = 1:5
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
%% Data Processing
Nfft = 128;
a = pwelch(test,boxcar(Nfft),0,Nfft,'twosided');
for shifting = 1:frames
    testLinear(:,shifting) = fftshift(a(:,shifting));
end
% Use from 21:108 and all odd carriers are noise, even carriers are signal
testLinearMod = testLinear(21:108,:); % Remove attenuated data
signal = testLinearMod(2:2:end,:);
noise = testLinearMod(1:2:end,:);
for samples = 1:frames
    for carriers = 1:44
        noiseAvg(carriers,samples) = sum(noise(:,samples)) / 44;
    end
end
testLinearSignal = signal ./ noiseAvg;
for carrierStats = 1:frames
    testMaxCarrier(carrierStats) = max(testLinearSignal(:,carrierStats));
    testMinCarrier(carrierStats) = min(testLinearSignal(:,carrierStats));
    testAvgCarrier(carrierStats) = sum(testLinearSignal(:,carrierStats)) / 44;
end
for sumCarriers = 1:44
    testSum(sumCarriers) = sum(testLinearSignal(sumCarriers,:));
end
testAvg = testSum / frames;
%% Plot Results
mx = max(testAvg); mn = min(testAvg); av = sum(testAvg) / 44;
figure(); stem(testAvg); title({'Average Carrier Strengths (linear)',...
    'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
figure();
mx = max(testMaxCarrier); mn = min(testMaxCarrier); av = sum(testMaxCarrier) / frames;
subplot(3,1,1); plot(testMaxCarrier); title({'Max Carrier Values Per Frame (linear)',...
    'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
mx = max(testMinCarrier); mn = min(testMinCarrier); av = sum(testMinCarrier) / frames;
subplot(3,1,2); plot(testMinCarrier); title({'Min Carrier Values Per Frame (linear)',...
    'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
mx = max(testAvgCarrier); mn = min(testAvgCarrier); av = sum(testAvgCarrier) / frames;
subplot(3,1,3); plot(testAvgCarrier); title({'Avg Carrier values Per Frame (linear)',...
    'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
clear; close all;
% programstart = datetime;
% waitduration = duration('00:00:18');
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
% while((datetime - programstart) < waitduration)
%     disp('waiting');
%     pause(1);
% end
numSpots = 10;
for locations = 1:numSpots
    beep;
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
    beep;
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
    
    AveragePerLocation(:,locations) = testAvg;
    MaxPerLocation(:,locations) = testMaxCarrier;
    MinPerLocation(:,locations) = testMinCarrier;
    AvgPerLocation(:,locations) = testAvgCarrier;
    pause(2);
end
%% Plot Results
for locations = 1:numSpots
    mx = max(AveragePerLocation(:,locations)); mn = min(AveragePerLocation(:,locations));
    av = sum(AveragePerLocation(:,locations)) / 44;
    figure(locations); stem(AveragePerLocation(:,locations)); title({'Average Carrier Strengths (linear)',...
        'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
    figure(locations+numSpots);
    mx = max(MaxPerLocation(:,locations)); mn = min(MaxPerLocation(:,locations));
    av = sum(MaxPerLocation(:,locations)) / frames;
    subplot(3,1,1); plot(MaxPerLocation(:,locations)); title({'Max Carrier Values Per Frame (linear)',...
        'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
    mx = max(MinPerLocation(:,locations)); mn = min(MinPerLocation(:,locations));
    av = sum(MinPerLocation(:,locations)) / frames;
    subplot(3,1,2); plot(MinPerLocation(:,locations)); title({'Min Carrier Values Per Frame (linear)',...
        'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
    mx = max(AvgPerLocation(:,locations)); mn = min(AvgPerLocation(:,locations));
    av = sum(AvgPerLocation(:,locations)) / frames;
    subplot(3,1,3); plot(testAvgCarrier); title({'Avg Carrier values Per Frame (linear)',...
        'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
end
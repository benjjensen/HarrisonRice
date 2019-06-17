% Continuously gathers data and plots it. Allows for a quick look at the
% channel

clearvars -except rx; close all;
% programstart = datetime;
% waitduration = duration('00:00:18');
warning('off','all')
%% Radio Setup
if (exist('rx','var') ~= 1)
    plutoradiosetup();
    rx = sdrrx('Pluto');
    rx.RadioID = 'usb:0';
    rx.CenterFrequency = 1250e6;
    rx.BasebandSampleRate = 20e6;
    rx.SamplesPerFrame = 32*64;
    rx.OutputDataType = 'double';
    rx.ShowAdvancedProperties = true;
end
%% Data Collection
ignore = zeros(2048,1);
for init = 1:100
    ignore(:,1) = rx();
end
frames = 2000;
test = zeros(2048,frames);
count = 0;
while(1)
    count = count + 1;
    for clearbuffer = 1:7
        buffer(:,1) = rx();
    end
    for runs = 1:frames
        test(:,runs) = rx();
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
    
    AveragePerLocation(:,count) = testAvg;
    MaxPerLocation(:,count) = testMaxCarrier;
    MinPerLocation(:,count) = testMinCarrier;
    AvgPerLocation(:,count) = testAvgCarrier;
    %% Plot Results
    mx = max(AveragePerLocation(:,count)); mn = min(AveragePerLocation(:,count));
    av = sum(AveragePerLocation(:,count)) / 44;
    figure(1); stem(AveragePerLocation(:,count)); title({'Average Carrier Strengths (linear)',...
        'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
    drawnow;
    pause(1);
%     figure(count+numSpots);
%     mx = max(MaxPerLocation(:,count)); mn = min(MaxPerLocation(:,count));
%     av = sum(MaxPerLocation(:,count)) / frames;
%     subplot(3,1,1); plot(MaxPerLocation(:,count)); title({'Max Carrier Values Per Frame (linear)',...
%         'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
%     mx = max(MinPerLocation(:,count)); mn = min(MinPerLocation(:,count));
%     av = sum(MinPerLocation(:,count)) / frames;
%     subplot(3,1,2); plot(MinPerLocation(:,count)); title({'Min Carrier Values Per Frame (linear)',...
%         'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
%     mx = max(AvgPerLocation(:,count)); mn = min(AvgPerLocation(:,count));
%     av = sum(AvgPerLocation(:,count)) / frames;
    %     subplot(3,1,3); plot(testAvgCarrier); title({'Avg Carrier values Per Frame (linear)',...
    %         'Max:'+string(mx)+' || Min:'+string(mn)+' || Avg:'+string(av)});
end
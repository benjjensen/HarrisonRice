% Continuously gathers data and plots it. Allows for a quick look at the
% channel to ensure things are working as expected

clear; close all
warning('off','all')
%% Radio Setup

    % Sets up the ADALM-Pluto Radios
plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

%% Data Collection

ignore = zeros(2048,1); % Solves a weird buffer problems we experience with the radios
for init = 1:100
    ignore(:,1) = rx();
end

frames = 400;
test = zeros(2048,frames);
count = 0;

while(1)
    count = count + 1;
    for clearbuffer = 1:7 % Solves a weird buffer problems we experience with the radios
        buffer(:,1) = rx();
    end
    for runs = 1:frames   % Gathers data
        test(:,runs) = rx();
    end
    beep;
    
    %% Data Processing
    
        % PWelch + FFT Shift
    Nfft = 128;
    a = pwelch(test,boxcar(Nfft),0,Nfft,'twosided');
    for shifting = 1:frames
        testLinear(:,shifting) = fftshift(a(:,shifting));
    end
    
    % Ignore edge carriers. All odd carriers are noise, even carriers are signal
    
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
end
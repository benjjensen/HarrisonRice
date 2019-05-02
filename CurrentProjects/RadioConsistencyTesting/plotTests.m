clear; close all;

Computer = input('Please enter your name\n','s');
TestNum = input('Enter the number of times that you have run this test (eg. "1", "2", etc.)\n');
for num = 1:12
    if num == 7
    else
        eval('agcname = ''Radio'' + string(num) + ''_'' + Computer + ''Auto'' + string(TestNum);');
        eval('manname = ''Radio'' + string(num) + ''_'' + Computer + ''Manual'' + string(TestNum);');
        loadname = 'Tests/' + agcname + '.mat';
        load(loadname);
        savename = sprintf('agc%d',num);
        eval(savename + " = " + agcname + ";");
        loadname = 'Tests/' + manname + '.mat';
        load(loadname);
        savename = sprintf('man%d',num);
        eval(savename + " = " + manname + ";");
        
    end
end
% load('Tests/Radio1_DakotaAuto0.mat');
% load('Tests/Radio1_DakotaManual0.mat');
% load('Tests/Radio2_DakotaAuto0.mat');
% load('Tests/Radio2_DakotaManual0.mat');
% load('Tests/Radio3_DakotaAuto0.mat');
% load('Tests/Radio3_DakotaManual0.mat');
% load('Tests/Radio4_DakotaAuto0.mat');
% load('Tests/Radio4_DakotaManual0.mat');
% load('Tests/Radio5_DakotaAuto0.mat');
% load('Tests/Radio5_DakotaManual0.mat');
% load('Tests/Radio6_DakotaAuto0.mat');
% load('Tests/Radio6_DakotaManual0.mat');
% load('Tests/Radio8_DakotaAuto0.mat');
% load('Tests/Radio8_DakotaManual0.mat');
% load('Tests/Radio9_DakotaAuto0.mat');
% load('Tests/Radio9_DakotaManual0.mat');
% load('Tests/Radio10_DakotaAuto0.mat');
% load('Tests/Radio10_DakotaManual0.mat');
% load('Tests/Radio11_DakotaAuto0.mat');
% load('Tests/Radio11_DakotaManual0.mat');
% load('Tests/Radio12_DakotaAuto0.mat');
% load('Tests/Radio12_DakotaManual0.mat');
% agc1 = Radio1_DakotaAuto0;
% man1 = Radio1_DakotaManual0;
% agc2 = Radio2_DakotaAuto0;
% man2 = Radio2_DakotaManual0;
% agc3 = Radio3_DakotaAuto0;
% man3 = Radio3_DakotaManual0;
% agc4 = Radio4_DakotaAuto0;
% man4 = Radio4_DakotaManual0;
% agc5 = Radio5_DakotaAuto0;
% man5 = Radio5_DakotaManual0;
% agc6 = Radio6_DakotaAuto0;
% man6 = Radio6_DakotaManual0;
% agc8 = Radio8_DakotaAuto0;
% man8 = Radio8_DakotaManual0;
% agc9 = Radio9_DakotaAuto0;
% man9 = Radio9_DakotaManual0;
% agc10 = Radio10_DakotaAuto0;
% man10 = Radio10_DakotaManual0;
% agc11 = Radio11_DakotaAuto0;
% man11 = Radio11_DakotaManual0;
% agc12 = Radio12_DakotaAuto0;
% man12 = Radio12_DakotaManual0;

frames = 200;
Nfft = 128;
for num = 1:12
    if num == 7
    else
        testagc = 'agc' + string(num);
        testman = 'man' + string(num);
        testagcLinear = 'agc' + string(num) + 'Linear';
        testmanLinear = 'man' + string(num) + 'Linear';
        testagcSignal = 'signal_agc' + string(num);
        testmanSignal = 'signal_man' + string(num);
        testagcNoise = 'noise_agc' + string(num);
        testmanNoise = 'noise_man' + string(num);
        testagcSum = 'agc' + string(num) + 'Sum';
        testmanSum = 'man' + string(num) + 'Sum';
        testagcCarriers = 'agc' + string(num) + 'carriers';
        testmanCarriers = 'man' + string(num) + 'carriers';
        eval(sprintf('titlestring = ''Radio Number %d'';',num));

        %% plot agc
        eval(sprintf('%s = pwelch(%s,boxcar(Nfft),0,Nfft,''twosided'');',testagc,testagc));
        for shifting = 1:frames
            eval(sprintf('%s(:,shifting) = fftshift(%s(:,shifting));',testagcLinear,testagc));
        end
        % Use from 21:108 and all odd carriers are noise, even carriers are signal
        eval(sprintf('%s = %s(21:108,:);',testagcLinear,testagcLinear));
        eval(sprintf('%s = %s(2:2:end,:);',testagcSignal,testagcLinear));
        eval(sprintf('%s = %s(1:2:end,:);',testagcNoise,testagcLinear));
        for samples = 1:frames
            for carriers = 1:44
                eval(sprintf('noiseAvg(carriers,samples) = sum(%s(:,samples))/44;',testagcNoise));
            end
        end
        eval(sprintf('%s = %s ./ noiseAvg;', testagcSignal,testagcSignal));        
        for sumCarriers = 1:44
            eval(sprintf('testSum(sumCarriers) = sum(%s(sumCarriers,:));',testagcSignal));
        end
        testAvg = testSum / frames;
        eval(sprintf('agc%d_avg = testAvg;',num));
        
        figure(num);
        subplot(2,1,1);
        hold on
        title('AGC')
        xlabel('Carrier Number')
        stem(testAvg);
        hold off
        
        figure(13);
        subplot(4,3,num);
        hold on
        title(titlestring);
        xlabel('Carrier Number')
        stem(testAvg);
        ylim([0,450]);

        hold off
        %% plot manual
        eval(sprintf('%s = pwelch(%s,boxcar(Nfft),0,Nfft,''twosided'');',testman,testman));
        for shifting = 1:frames
            eval(sprintf('%s(:,shifting) = fftshift(%s(:,shifting));',testmanLinear,testman));
            %             agc1Linear(:,shifting) = fftshift(testagc(:,shifting));
        end
        % Use from 21:108 and all odd carriers are noise, even carriers are signal
        eval(sprintf('%s = %s(21:108,:);',testmanLinear,testmanLinear));
        eval(sprintf('%s = %s(2:2:end,:);',testmanSignal,testmanLinear));
        eval(sprintf('%s = %s(1:2:end,:);',testmanNoise,testmanLinear));
        for samples = 1:frames
            for carriers = 1:44
                eval(sprintf('noiseAvg(carriers,samples) = sum(%s(:,samples))/44;',testmanNoise));
            end
        end
        eval(sprintf('%s = %s ./ noiseAvg;', testmanSignal,testmanSignal));
        
        for sumCarriers = 1:44
            eval(sprintf('testSum(sumCarriers) = sum(%s(sumCarriers,:));',testmanSignal));
        end
        testAvg = testSum / frames;
        eval(sprintf('man%d_avg = testAvg;',num));
        figure(num)
        subplot(2,1,2);
        hold on
        title('Manual Gain');
        stem(testAvg);
        xlabel('Carrier Number')
        hold off
        sgtitle(titlestring);   
        
        figure(14);
        subplot(4,3,num);
        hold on
        title(titlestring);
        xlabel('Carrier Number')
        stem(testAvg);
        ylim([0,150]);
        hold off
    end

    figure(13)
    sgtitle('AGC');
    figure(14)
    sgtitle('Manual');
end
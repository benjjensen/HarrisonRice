%%% subPlotDopplerPowerSpectrum %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This is a modified version of makeDopplerPowerSpectrum.m, used to
% generate a figure for the 2019 ITC paper. 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Fs = 11.2e3; % sample rate of input file
Nfft = 1024; % FFT length
skip = 20*Fs; % eliminate the bad data at the beginning

% Array of the four single-carrier data file paths
    % Note - Charlie-2 is bad, but charlie-1 is worse
    % These may need to be shifted. This can be done below in the for loop
inFilename = ["Data/ProcessedData/beta-3_downsample.mat", "Data/ProcessedData/charlie-2_downsample.mat" ...
    "Data/ProcessedData/delta-1_downsample.mat", "Data/ProcessedData/echo-1_downsample.mat"];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

for file = 1:4
    load(inFilename(file)); % Loads in y
    FF = (-0.5:1/Nfft:0.5-1/Nfft)';
    
% Use the following line if you do not want to manually shift
    YY = pwelch(y,boxcar(Nfft),0,Nfft,'two-sided');
    
% Use the following two lines if you do want to manually shift towards center
%     yTemp = y(skip+1:end); 
%     YY = pwelch(yTemp,boxcar(Nfft),0,Nfft,'two-sided');
    

    YYshift = fftshift(YY);
    
    % find the indexes for of all DPS components greater than
    % the DC value
    
    idx4avg = find(YYshift > YYshift(Nfft/2+1));
    
    % compute frequency offset by averaging over those values
    
    Foffset = FF(idx4avg)'*YYshift(idx4avg)/sum(YYshift(idx4avg));
    
    % remove the frequency offset
    
    y2 = y.*exp(-1i*2*pi*Foffset*(0:length(y )-1)');
    
    % plot the DPS of the frequency-corrected data
    
    YY2 = pwelch(y2,boxcar(Nfft),0,Nfft,'two-sided');
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
    secondsPerBlock = 1;
    samplesPerBlock = fix(secondsPerBlock*Fs/Nfft) * Nfft;
    secondsPerBlock = samplesPerBlock/Fs;
    numBlocks = ceil(length(y)/samplesPerBlock);
    
    YYm = zeros(Nfft,numBlocks);
    for idx = 1:numBlocks
        if idx*samplesPerBlock > length(y2)
            temp = y2((idx-1)*samplesPerBlock+1:end);
        else
            temp = y2((idx-1)*samplesPerBlock+1:idx*samplesPerBlock);
        end
        YYm(:,idx) = fftshift(pwelch(temp,boxcar(Nfft),0,Nfft,'two-sided'));
    end
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
    figure(1);
    subplot(2,2,file);
    contourf(FF*Fs,(1:numBlocks)*secondsPerBlock,10*log10(YYm'));
    grid on;
    ax = gca;
    colorbar;
    ax.Children.LineStyle = 'none';
    ax.XLim = [-500 500];
    ax.XLabel.String = 'frequency (Hz)';
    ax.YLabel.String = 'time (s)';
%     saveas(gcf,'DopplerPowerSpectrums','epsc');
    
    figure(2);
    subplot(1,4,file);
    contourf(FF*Fs,(1:numBlocks)*secondsPerBlock,10*log10(YYm'));
    grid on;
    ax = gca;
    ax.Children.LineStyle = 'none';
    ax.XLim = [-500 500];
    ax.XLabel.String = 'frequency (Hz)';
    ax.YLabel.String = 'time (s)';
    if (file == 4)
      colorbar;
    end
%     saveas(gcf,'DopplerPowerSpectrums','epsc');
end

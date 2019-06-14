%%% makeDopplerPowerSpectrum

Fs = 11.2e3; % sample rate of input file
Nfft = 1024; % FFT length

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% input files
%%% you will probably have different file names and different paths
%%% modify as necessary
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

inFilename = 'Data/ProcessedData/trimmed_delta-1_downsample.mat';
skip = 15*Fs; % eliminate the bad data at the beginning

% inFilename = 'singleCarrierData/echo_2019-03-19__14-26-56_downsample.mat';
skip = 10*Fs; % eliminate the bad data at the beginning

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

load(inFilename); % y 

        % We are using the corrected data     
% y = y(skip+1:end); % this will not be necessay if you use the files
                   % Nathan has corrected.

FF = (-0.5:1/Nfft:0.5-1/Nfft)';

YY = pwelch(y,boxcar(Nfft),0,Nfft,'two-sided');
figure(1); subplot(211); plot(FF,fftshift(YY)); grid on;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% find the average frequency offset
%%% the method is huristic, but it seems to work well
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
figure(1); subplot(212); plot(FF,fftshift(YY2)); grid on;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% create the data for the contour
%%% by computing the PSD for each block of secondsPerBlock data
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% make the plots
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

figure();
contourf(FF*Fs,(1:numBlocks)*secondsPerBlock,10*log10(YYm'));
grid on;
ax = gca;
colorbar;
ax.Children.LineStyle = 'none';
ax.XLim = [-500 500];
ax.XLabel.String = 'frequency (Hz)';
ax.YLabel.String = 'time (s)';

figure(4);
waterfall(FF*Fs,(1:numBlocks)*secondsPerBlock,10*log10(YYm'));
grid on;
ax2 = gca;
ax2.XLim = [-500 500];
ax2.XLabel.String = 'frequency (Hz)';
ax2.YLabel.String = 'time (s)';

figure(6);
plot(FF*Fs,10*log10(sum(YYm,2)));
grid on;
ax3 = gca;
ax3.XLim = [-500 500];
ax3.XLabel.String = 'frequency (Hz)';
ax3.YLabel.String = 'magnitude (dB)';
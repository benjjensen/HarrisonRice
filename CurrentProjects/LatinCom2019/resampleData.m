%%% resampleData

%%% heterodynes the raw A/D data to baseband and resamples from the original
%%% 100 Msamples/s to 20 Msamples/s

% inFilename = 'Sundance-To-US189-25June2019_file1.mat';
% outFilename = '/media/V2V/Sundance-To-US189-25June2019_20Msps_file1.mat';
% outFilename = '/media/V2V/Sundance-To-US189-25June2019_20Msps.mat'; % Down the canyon
outFilename = '/media/V2V/US189-To-Sundance-25June2019_20Msps.mat'; % Up the canyon

Ndata = 10000000; % Originally was 160000

%%% downsample by 5

%%% get resampling filter
[~,h] = resample(randn(100,1),1,5);

%%% create downsample system object
firds = dsp.FIRDecimator(5,h);

%%% create vector for heterodyne
eetemp = exp(1i*2*pi*0.3*(0:9)');
ee = repmat(eetemp,Ndata/length(eetemp),1);

%%% read and write file objects
% mfr = dsp.MatFileReader(inFilename,'VariableName','data','SamplesPerFrame',Ndata);
mfw = dsp.MatFileWriter(outFilename,'VariableName','y');

%%% load raw data, Ndata samples at a time, resample, and write resampled
%%% data in output file

% x2 = mfr();
% x1 = (x2-8192)/8192;
% x = x1 .* ee;
% y = firds(x);
% mfw(y);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% plot the first psd of the first Ndata samples
%%% (this is optional)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Nfft = 2048;
% FF = -0.5:1/Nfft:0.5-1/Nfft;
% XX1 = pwelch(x1,blackman(Nfft),Nfft/2,Nfft,'twosided');
% YY = pwelch(y,blackman(Nfft),Nfft/2,Nfft,'twosided');
% 
% figure(1);
% subplot(211); 
% plot(FF*100,10*log10(fftshift(XX1))); grid on;
% ax1 = gca;
% subplot(212);
% plot(FF*20,10*log10(fftshift(YY))); grid on;
% ax2 = gca;
% drawnow;
% 
% ax1.XLabel.String = 'frequency (MHz)';
% ax1.YLabel.String = 'input PSD (dB)';
% ax2.XLabel.String = 'frequency (MHz)';
% ax2.YLabel.String = 'resampled PSD (dB)';

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% end optional
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% New Section (uncomment out everything else if not using this)
%%%%%%%%%%%%%%%%%%%%%%%%%%%

numFiles = 2;
count = 0;
outFilenum = 1;

disp(datetime);
for i = 1:numFiles
    inFilename = ['/media/V2V/US189-To-Sundance-25June2019_file' num2str(i) '.mat'];
    mfr = dsp.MatFileReader(inFilename,'VariableName','data','SamplesPerFrame',Ndata);

    while ~isDone(mfr)
        count = count + 1;
        x2 = mfr();
        if (isDone(mfr) && (i == numFiles))
            % Do nothing to ignore the last bit of the last file
        else
            x1 = (x2-8192)/8192;
            x = x1 .* ee;
            y = firds(x);
            mfw(y);
%             if count == 180
%                 release(mfw);
%                 outFilenum = outFilenum + 1;
%                 outFilename = ['/media/V2V/Sundance-To-US189-25June2019_20Msps_file' num2str(outFilenum) '.mat'];
%                 mfw = dsp.MatFileWriter(outFilename,'VariableName','y');
%                 count = 0;
%             end
        end
    end
release(mfr);
disp('Finished one file');
disp(datetime);
end
disp('Done with ResampleData');
%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% End New Section
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% while ~isDone(mfr)
%     x2 = mfr();
%     x1 = (x2-8192)/8192;
%     x = x1 .* ee;
%     y = firds(x);
%     mfw(y);
% end
% release(mfr);
release(mfw);   

% ProcessResampled;
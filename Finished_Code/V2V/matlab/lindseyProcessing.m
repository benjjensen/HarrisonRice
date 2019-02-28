%%% lindseyProcessing

Fs = 11.2;
Fs2 = Fs/1000;
F0 = 0.25;
L = 200;

% process one second at a time, non-overlapping

N1 = Fs*1e6;
N2 = N1/1000;

% lowpass filter
h = firls(L,[0 0.01 0.02 0.5]*2,[1 1 0 0]);
firdownsample = dsp.FIRDecimator(1000,h);

% file
% inFilename = '/media/V2V/2019-02-21_data-collection/test-point-E-Lindsey_2019-02-21__13-55-39.mat';
 inFilename = '/media/V2V/2019-02-21_data-collection/test-point-D-Lindsey_2019-02-21__13-47-21.mat';
% inFilename = '/media/V2V/2019-02-21_data-collection/test-point-C-Lindsey_2019-02-21__13-39-03.mat';


mfr = dsp.MatFileReader(inFilename,'VariableName','data','SamplesPerFrame',N1);

% fft
Nfft = 2048; % frequency resoltion = 11200/2048 = 4.46875 Hz

% downconverter sinusoid
ee = exp(-1i*pi/2*(0:N1-1)');

Nsec = 30;
S = zeros(Nsec,Nfft);

for idx = 1:Nsec
    fprintf(1,'working on second %d\n',idx);
    temp = mfr();
    x = (temp - 8192)/8192;
    x2 = x.*ee;
    y = firdownsample(x2);
    S(idx,:) = fftshift(pwelch(y,blackman(Nfft),0.5*Nfft,Nfft,'two-sided'));
end
release(mfr);

%%
FF = (-0.5:1/Nfft:0.5-1/Nfft)*Fs2*1e6;
figure(1); contourf(FF,1:Nsec,10*log10(S)+100); colorbar;


figure(2); surf(FF,1:Nsec,10*log10(S)+100);
shading interp;

figure(3); clf;
plot(FF,10*log10(S(1,:))+100); grid on; ax = gca;
ax.YLim = [20 80];
ax.Title.String = 'time = 1 s';
for idx = 2:Nsec
    ax.Children.YData = 10*log10(S(idx,:))+100;
    ax.Title.String = sprintf('time = %d s',idx);
    pause(0.25);
end
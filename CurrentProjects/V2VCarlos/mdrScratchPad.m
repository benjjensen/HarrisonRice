%%% mdrScratchPad

inFilename = '/media/V2V/dry-run_2019-03-18__18-36-24.mat';
m = matfile(inFilename);
Nfft = 16*128;
FF = -0.5:1/Nfft:0.5-1/Nfft;
temp = m.data(1:1000*128*10,1);
x = (temp - 8192)/8192;
XX = pwelch(x,boxcar(Nfft),0,Nfft,'two-sided');

figure(999);
plot(FF,fftshift(XX));
grid on;

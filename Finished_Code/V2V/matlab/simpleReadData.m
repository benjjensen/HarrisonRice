% clear all

data = readBatched_DAQshortSamples_host('../acquire/uvdma.dat');

figure(1); clf
plot(1:40,data(1:40)-9260.7,'.-',...
    1:40,data(41:80)-9260.7,'-.',...
    1:40,data(81:120)-9260.7,'.-'); grid on;

Nfft = 1000;
PP = pwelch(data(1:500000)-9260.7,ones(1,Nfft),0,Nfft,'twosided');
FF = -0.5:1/Nfft:0.5-1/Nfft;
figure(2);
plot(FF*100,10*log10(fftshift(PP))); grid on;
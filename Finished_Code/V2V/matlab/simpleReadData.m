% clear all

data = readBatched_DAQshortSamples_host('/media/V2V/continuous_fm_2018-08-13__09-51-59.dat');

figure(1); clf
plot(1:300,data(1:300)-8180.7,'.-'); grid on;%,...
    %1:300,data(301:600)-8180.7,'-.',...
    %1:300,data(601:900)-8180.7,'.-'); grid on;

Nfft = 1000;
PP = pwelch(data(1:500000)-8180.7,ones(1,Nfft),0,Nfft,'twosided');
FF = -0.5:1/Nfft:0.5-1/Nfft;
figure(2);
plot(FF*100,10*log10(fftshift(PP))); grid on;
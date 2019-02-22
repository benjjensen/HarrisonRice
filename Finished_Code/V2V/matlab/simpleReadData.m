% clear all

data = readBatched_DAQshortSamples_host('/media/V2V/PlutoTest_2018-10-08__16-21-17.dat');

figure(1); clf
plot(1:10,data(1:10),'.-',...
    1:10,data(11:20),'-.',...
    1:10,data(21:30),'.-'); grid on;

Nfft = 1000;
PP = pwelch(data(1:500000)-8180.7,ones(1,Nfft),0,Nfft,'twosided');
FF = -0.5:1/Nfft:0.5-1/Nfft;
figure(2);
plot(FF*100,10*log10(fftshift(PP))); grid on;
%%% mdrQuickTest

% load D:\Test-_2019-08-26__12-57-22.mat % data

Nfft = 2048;
FF = -0.5:1/Nfft:0.5-1/Nfft;

YY = pwelch(data(1:200000),blackman(Nfft),Nfft/2,Nfft,1,'twosided');
plot(FF,10*log10(YY)); grid on;
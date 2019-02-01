close all;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
% EveryOther(64,samples) = zeros();
% Averages(1,samples) = zeros();

for runs = 1:10
    YY = pwelch(test(:,runs),boxcar(Nfft),0,Nfft,'twosided');
    YYplot = 10*log10(abs(fftshift(YY)));
    figure(runs);
    stem(FF,YYplot);
    grid on;
    xlabel('frequency (MHz)');
    ylabel('Magnitude (dB)');
    ax = gca;
    ax.Children.BaseValue = -80;
    hold on;
    plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
    hold off;
%     EveryOther(:,runs) = YYplot(1:2:end,:);
%     Averages(1,runs) = mean(EveryOther(:,runs));
end


function [] = Graphing(dataArray,rawData,closeOtherFigures)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
if (closeOtherFigures)
    close all;
end
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
[X,Y,Z] = size(dataArray);
if (Y * Z > 20)
    numPlots = 20;
    if (Z > 5)
        numRows = 4;
        numCols = 5;
    else
        numCols = Z;
        numRows = 20 / Z;
    end
else
    numPlots = Y * Z;
    numRows = Y;
    numCols = Z;
end

if (rawData)
    for runs = 1:numPlots
        YY = pwelch(dataArray(:,runs),boxcar(Nfft),0,Nfft,'twosided');
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
    end
else
    for Row = 1:numRows
        for Col = 1:numCols
            YYplot = dataArray(:,Row,Col);
            figure();
            stem(FF,YYplot);
            title('(' + string(Row) + ',' + string(Col) + ')');
            grid on;
            xlabel('frequency (MHz)');
            ylabel('Magnitude (dB)');
            ax = gca;
            ax.Children.BaseValue = -80;
            hold on;
            plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
            hold off;
        end
    end
end
end


function [] = GraphingV2V(dataArray,closeOtherFigures, convertdB)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
if (closeOtherFigures)
    close all;
end

Nfft = 64; % Number of sub-carriers
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


for Row = 1:numRows
    for Col = 1:numCols
        if (convertdB)
            YYplot = 10*log10(abs(dataArray(Row,:,Col)));
        else
            YYplot = dataArray(Row,:,Col);
        end
        figure();
        stem(FF,YYplot);
        title('(' + string(Row) + ',' + string(Col) + ')');
        grid on;
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        if (convertdB)
            ax.Children.BaseValue = -80;
        end
    end
end

end


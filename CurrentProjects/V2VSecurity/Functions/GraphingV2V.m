function [] = GraphingV2V(dataArray,closeOtherFigures)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
if (closeOtherFigures)
    close all;
end

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
        YYplot = dataArray(Row,:,Col);
        figure();
        stem(FF,YYplot);
        title('(' + string(Row) + ',' + string(Col) + ')');
        grid on;
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        ax.Children.BaseValue = -80;
        hold on;
%         plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
        hold off;
    end
end

end


function [graphWeights] = HammingWeightHier(m, graph)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

if 2 <= m
    
    n = (2^m) - 1;
    k = n-m;
    
    rate = k/n;
    
    nValues= zeros(1,n);
    for i = 1 : n
        nValues(i) = i;
    end
    
    for i = 1 : n
        for j = 0 : m
            if nValues(i) == 2^j
                nValues(i) = -1;
            end
        end
    end
    nValuesSort = sort(nValues);
    
    firstIndex = 0;
    for i = 1 : n
        if (nValuesSort(i) == -1) && (nValuesSort(i+1) ~= -1)
            firstIndex = i+1;
        end
    end
    
    weights = nValuesSort(firstIndex :end);
    
    weightsIndex = 1;
    graphWeights = zeros(1,n+1);
    for index = 1 : n + 1
        if index <= weights(1,weightsIndex)
            graphWeights(1,index) = k - weightsIndex + 1;
        elseif index > weights(1, weightsIndex)
            weightsIndex = weightsIndex + 1;
            graphWeights(1,index) = k - weightsIndex + 1;
        end
    end
    
    graphIndexes = zeros(1,n+1);
    for j = 0 : n
        graphIndexes(1, j+1) = j;
    end
    
    if graph == true
        figure();
        plot(graphIndexes,graphWeights,'DisplayName',['Hamming(' num2str(m) '), rate = ' num2str(round(rate,2))]);
        grid on;
        title({['\fontsize{12}Security Curve for Hamming(' num2str(m) ')'] ; ...
            ['\fontsize{11}n = ' num2str(n) ', k = ' num2str(k)]});
        ylabel('Equivocation (bits)');
        xlabel ('Revealed Bits (\mu)');
        legend;
    end
else
    printf('Cannot produce Hamming Code with given Parameters');
end


end

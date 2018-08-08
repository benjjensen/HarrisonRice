function [graphWeights] = SimplexWeightHier(m, graph)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

if 2 <= m
    
    n = (2^m) - 1;
    Simplex_k = n-m;
%     k = n-k_IT;
    
    rate = Simplex_k/n;
    
    nValues= zeros(1,n);
    for i = 1 : n
        nValues(i) = i;
    end
    
    for i = 1 : n
        for j = 0 : m-1
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
%     weights = sort(n + 1 - weightsComp);
    
    weightsIndex = 1;
    graphWeights = zeros(1,n+1);
    for index = 1 : n + 1
        if index <= weights(1,weightsIndex)
            graphWeights(1,index) = Simplex_k - weightsIndex + 1;
        elseif index > weights(1, weightsIndex)
            weightsIndex = weightsIndex + 1;
            graphWeights(1,index) = Simplex_k - weightsIndex + 1;
        end
    end
    
    graphIndexes = zeros(1,n+1);
    for j = 0 : n
        graphIndexes(1, j+1) = j;
    end
    
    if graph == true
        figure();
        plot(graphIndexes,graphWeights,'DisplayName',['Simplex(' num2str(m) '), rate = ' num2str(round(rate,2))]);
        grid on;
        title({['\fontsize{12}Security Curve for Simplex(' num2str(m) ')'] ; ...
            ['\fontsize{11}n = ' num2str(n) ', k = ' num2str(Simplex_k)]});
        ylabel('Equivocation (bits)');
        xlabel ('Revealed Bits (\mu)');
        legend;
    end
else
    printf('Cannot produce Hamming Code with given Parameters');
end


end

%This file is an object for each of the codes. It is intended for use with
%codeInfoGraph.m
classdef codeInfo
    properties
        u %parameter of a Reed-Muller code
        m %parameter of a reed-Muller code
        n %number of bits in a code word
        k %number of bits in a symbol
        rate %rate of the code
        dBLevel
        mu %revealed bits
        H %equivocation
        percentLeaked %percent of the word leaked to Eve
        carrierRate %rate of the code multiplied by the max number of carriers used
        percentH %percent euivocation
    end
end


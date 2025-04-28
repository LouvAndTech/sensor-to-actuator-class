FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu

# Install zsh and oh-my-zsh
RUN apt-get update && \
    apt-get install -y zsh wget git && \
    wget https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O - | zsh || true

# Set zsh as the default shell
RUN chsh -s $(which zsh)

# Install Node.js (version 16 or higher)
RUN apt-get update && \
    curl -fsSL https://deb.nodesource.com/setup_16.x | bash - && \
    apt-get install -y nodejs

RUN apt-get update && \
    apt-get install -y texlive-latex-base \
                       texlive-latex-extra \
                       texlive-extra-utils \
                       texlive-font-utils \
                       texlive-lang-french \
                       texlive-lang-english \
                       biber \
                       texlive-bibtex-extra \
                       python3-pygments \
                       latexmk \
                       fonts-open-sans \
                       graphviz \
                       librsvg2-bin \
                       openjdk-11-jdk \
                       openjdk-11-jre && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

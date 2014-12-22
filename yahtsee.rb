require 'formula'

class Yahtsee < Formula
  homepage 'http://github.com/c0der78/terminal-yahtsee'
  url 'https://github.com/c0der78/terminal-yahtsee.git'

  head 'http://github.com/c0der78/terminal-yahtsee.git'
	
  version '0.1'

  depends_on 'arg3'

  def install
    ENV.universal_binary
    
    system "./configure", "--prefix=#{prefix}"
    ENV.deparallelize
    system "make install"
  end
end

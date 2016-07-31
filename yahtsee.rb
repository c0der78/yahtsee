require 'formula'

class Yahtsee < Formula
  homepage 'http://github.com/ryjen/yahtsee'
  url 'https://github.com/ryjen/yahtsee.git'

  head 'http://github.com/ryjen/yahtsee.git'
	
  version '0.1'

  depends_on 'libryjen'

  def install
    ENV.universal_binary
    
    system "./configure", "--prefix=#{prefix}"
    ENV.deparallelize
    system "make install"
  end
end

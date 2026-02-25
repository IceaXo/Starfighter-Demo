// bloom.fs
#version 330

// GPU 传进来的纹理坐标和原始贴图
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;// [全局输入] CPU 传进来的那张“隐形画布”贴图

void main()
{
    // 1. 获取当前像素的原始颜色
    vec4 texel = texture(texture0, fragTexCoord);
    
    // 2. 极其粗暴的泛光算法：由于你用了加法混合，发光的地方非常亮
    // 我们向四周采样几个点，叠加上去形成光晕
    vec4 bloom = vec4(0.0);
    float offset = 0.01; // 光晕扩散范围
    
    bloom += texture(texture0, fragTexCoord + vec2(offset, 0.0));
    bloom += texture(texture0, fragTexCoord + vec2(-offset, 0.0));
    bloom += texture(texture0, fragTexCoord + vec2(0.0, offset));
    bloom += texture(texture0, fragTexCoord + vec2(0.0, -offset));
    bloom += texture(texture0, fragTexCoord + vec2(offset, offset));
    bloom += texture(texture0, fragTexCoord + vec2(-offset, -offset));
    
    // 3. 将原色和模糊后的光晕混合 (除以6求个平均，再稍微放大亮度)
    finalColor = texel + (bloom / 6.0) * 3.;
}
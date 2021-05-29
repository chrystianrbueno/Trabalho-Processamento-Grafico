#pragma once

//Esta classe vai ir sofrendo altera��es durante o curso
//Aqui ela est� preparada apenas para sprites texturizados

#include "Shader.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sprite
{
public:
	Sprite();
	~Sprite() {}

	void initialize(); //similar ao setupGeometry

	//Para setar os atributos 
	void setTexture(int texID);
	void setShader(Shader* shader) { this->shader = shader; shader->Use(); }
	void setDimension(glm::vec3 scale) { this->scale = scale; }
	void setAngle(float angle) { this->angle = angle; } //para 3D precisa angulo por eixo ou quaternion

	void setPosX(float posX)
	{
		this->posX = posX;
	}

	void setPosY(float posY)
	{
		this->posY = posY;
	}

	void setPosZ(float posZ)
	{
		this->posZ = 0.0f;
	}

	void setTempoQueda(float tempoQueda)
	{
		this->tempoQueda = tempoQueda;
	}

	float getPosX() 
	{
		return posX;
	}

	float getPosY() 
	{
		return posY;
	}

	float getTempoQueda()
	{
		return tempoQueda;
	}

	//Para controle direto da matriz de transforma��es
	void setRotation(float angle, glm::vec3 axis, bool reset = true);
	void setTranslation(glm::vec3 displacements, bool reset = true);
	void setScale(glm::vec3 scaleFactors, bool reset = true);
	
	//Chamados no ciclo do SceneManager
	void draw();
	void update();
	void deleta();

	//TO DO: m�todos para anima��o

protected:
	//Atributos gerais

	GLuint VAO; //ID do buffer de geometria

	glm::mat4 transform; //matriz de transforma��o

	unsigned int texID; //identificador da textura

	Shader* shader; //ponteiro para o objeto de shader, para fazer a interface

	//Atributos de controle da posi��o, orienta��o e dimens�es do sprite
	glm::vec3 pos, scale;
	float posX, posY, posZ;
	float angle;
	float tempoQueda;

	//Atributos para controle da anima��o
	int nAnims, nFrames;
	int iAnim, iFrame; //indices da anima��o e frame atual

};


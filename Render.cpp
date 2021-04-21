#include "Render.h"

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}


GLuint Render::generateVAO() {
	GLuint VAO = 0;
	glGenVertexArrays(1, &VAO);
	if (!VAO)
		throw std::runtime_error(std::string("Error setting VAO number: ") + std::to_string(count));
	return VAO;
};

GLuint Render::generateVBO() {
	GLuint VBO = 0;
	glGenBuffers(1, &VBO);
	if (!VBO)
		throw std::runtime_error(std::string("Error setting VBO number: ") + std::to_string(count));
	return VBO;
};

GLuint Render::generateEBO() {
	GLuint EBO = 0;
	glGenBuffers(1, &EBO);
	if (!EBO)
		throw std::runtime_error(std::string("Error setting VBO number: ") + std::to_string(count));
	return EBO;
};

Render::Render(Shader* shaderInstance) : shaderinstance(shaderInstance) {
	count = 0;
	/*
	Instansiate render class without any objects to render
	*/
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDebugMessageCallback(MessageCallback, 0);
	shaderInstance->use();
	shaderInstance->setMat4("model", glm::mat4(1.0f));
}

Render::~Render() {
	glBindVertexArray(0);
	//DELETE ALL INSTANCES (and data internally) of objectDATA
	for (auto it : data_list) {
		//Delete all data inside objectData!
		glDeleteVertexArrays(1, &it->Attribs.VAO);
		glDeleteBuffers(1, &it->Attribs.VBO);
		glDeleteBuffers(1, &it->Attribs.EBO);
		clean(&it->Attribs.TAO);
		delete it;
	}
}

bool Render::FromData(std::string name, std::string texture, std::vector<GLfloat> data, std::vector<GLshort> indices, type type, bool Static=1, decltype(GL_TRIANGLES) drawtype= GL_TRIANGLES) {
	objectData* myObjectData = new objectData; //Preallocate data for the object
	//set data type here!
	myObjectData->ID = count;
	myObjectData->shader = shaderinstance; // Set shader!
	myObjectData->drawType = drawtype;

	std::cout << "Formatting vertices...";
	myObjectData->vertices = n_formatData(data, type); //Format data into positions.
	if (myObjectData->vertices.size() == 0) {
		std::cout << "Err, vertices could not be formatted";
		return false;
	}
	std::cout << "Successfully formatted vertices\n";
	myObjectData->Attribs.VAO = generateVAO();
	bindVertexObject(myObjectData->Attribs.VAO);

	myObjectData->Attribs.VBO = generateVBO();
	bindBuffers(myObjectData->Attribs.VBO, data); //CREATE TWO COPIES 1 for VAO, positions etc, other for myObjData type!
	
	vertexAttribute(0, 3, type, 0); //position attribute
	if (type != pc){
		myObjectData->Attribs.TAO = texture != "" ? newTexture(texture) : emptyTex;
		vertexAttribute(2, 2, type, 3); //texture attribute
		if (type != pt) {
			std::cout << "Data format type ptc assumed\n";
			vertexAttribute(1, 4, type, 5); //colour
		}
		if (type == ptcn) {
			std::cout << "Data format type ptcn assumed\n";
			vertexAttribute(3, 3, type, 9); //Normal attribute
		}
	}
	else {
		std::cout << "Data format type pc assumed\n";
		vertexAttribute(1, 4, type, 3);
	}

	std::cout << "Binding EBO...";
	if (indices.size()) {
		myObjectData->Attribs.EBO = generateEBO();
		myObjectData->indices = indices;
		bindIndices(myObjectData->Attribs.EBO, indices); //CREATE TWO COPIES 1 for VAO, positions etc, other for myObjData type!
		std::cout << "Successfully bound EBO";
	}
	else
		std::cout << " Indices vector empty, no EBO to bind";
	bindVertexObject(); // unbind vao's/vbo's
	pub_objects[name.c_str()] = *myObjectData;

	data_list.push_back(myObjectData);
	count += 1; //Count of all objects
	return true; //Creating object worked!
}


std::vector<glm::vec3> Render::n_formatData(std::vector<GLfloat> data, type type) {
	/*
	gets vertices from data and puts into convienent data format for easy manipulation
	*/
	std::vector<glm::vec3> myVertices;
	myVertices.reserve(data.size());
	for (int it = 0; it < data.size();) {
		myVertices.push_back(glm::vec3(data[it],data[it+1],data[it+2]));
		it += type;
	}

	return myVertices;
}


// ORDER OF OPERATION, BIND VAO, BIND VBO TO VAO, unbind repeat
// FOR DRAWING, bind vao, draw, unbind, draw anew


// CREATE BINDBUFFERS FOR NON INDICES OBJ
// CREATE FUNCTION FOR DETERMINING AMOUNT OF ATTRIBS FOR DATA (POS, COL, TEXT, NORM, ETC) possibly based on num attributes in shader program??????
// Seperate buffers for positions, colours, and textures!
void Render::bindVertexObject(GLuint VAO) {
	glBindVertexArray(VAO);
	// error check!
};
void Render::bindVertexObject() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Render::bindBuffers(GLuint& VBO, std::vector<GLfloat>& data) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data[0]) * (data.size()), data.data(), GL_STATIC_DRAW);
};
void Render::bindIndices(GLuint EBO, std::vector<GLshort> indices) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices.data()[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
};
void Render::vertexAttribute(int indx, int size, int stride, int offset) {
	glVertexAttribPointer(indx, size, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(offset*sizeof(float)));
	glEnableVertexAttribArray(indx); //Index of colour attribute
}

void Render::update() { // Updates the local coordinates
	for (auto it : data_list){
		if (it->CoordSystem.change) {
			for (int i = 0; i < it->vertices.size(); i++) 
				updateVertices(it->vertices[i], it->CoordSystem.model);
			}
	}
}
void Render::Rotate(std::string name, float angle, glm::vec3 axis) {// THIS WORKS!!!!!
	//Rotate object on GPU
	objectData& p_obj = *data_list[pub_objects[name.c_str()].ID];
	p_obj.CoordSystem.rotation = Rotate_vertices(angle, axis, p_obj.CoordSystem.model, p_obj.CoordSystem.rotation);
																	
	//Rotate object
	p_obj.shader->use();
	p_obj.shader->setMat4("model", p_obj.CoordSystem.model);
}
void Render::Translate(std::string name, glm::vec3 pos) { // THIS WORKS!!!!!!!!!!!!!
	//Translate object on GPU
	objectData& p_obj = *data_list[pub_objects[name.c_str()].ID];
	p_obj.CoordSystem.translation=Translate_vertices(pos, p_obj.CoordSystem.model, p_obj.CoordSystem.translation);
	//Translate object on CPU
	p_obj.shader->use();
	p_obj.shader->setMat4("model", p_obj.CoordSystem.model);
}
void Render::Scale(std::string name, float size) { //THIS WORKS
	//Scale object on GPU
	objectData& p_obj = *data_list[pub_objects[name.c_str()].ID];
	p_obj.CoordSystem.scale=Scale_vertices(size, p_obj.CoordSystem.model,p_obj.CoordSystem.scale);
	//Scale object on CPU
	p_obj.shader->use();
	p_obj.shader->setMat4("model", p_obj.CoordSystem.model);
}
void Render::Scale(std::string name, glm::vec3 size) { //THIS WORKS
	//Translate object on GPU
	objectData& p_obj = *data_list[pub_objects[name.c_str()].ID];
	p_obj.CoordSystem.scale = Scale_vertices(size, p_obj.CoordSystem.model, p_obj.CoordSystem.scale);
	//Scale object on CPU
	p_obj.shader->use();
	p_obj.shader->setMat4("model", p_obj.CoordSystem.model);
}

bool Render::updateTexture(std::string textureName, unsigned int texID) {
	clean(&texID);
	texID = NULL;
	texID = newTexture(textureName);
	if (!texID) {
		std::cerr << "Err new texture could not be bound";
		return false;
	}
	return true;
}

// NEEDS UPDATING!!!
void Render::loop() {
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shaderinstance->use();
	for (int i = 0; i < data_list.size(); i++) {
		shaderinstance->setMat4("model", data_list[i]->CoordSystem.model); //Transform each individually
		glBindVertexArray(data_list[i]->Attribs.VAO);
		if (data_list[i]->Attribs.TAO)
			glBindTexture(GL_TEXTURE_2D, data_list[i]->Attribs.TAO);
		if (data_list[i]->Attribs.EBO)
			glDrawElements(data_list[i]->drawType, data_list[i]->indices.size(), GL_UNSIGNED_SHORT, 0);
		else
			glDrawArraysInstanced(data_list[i]->drawType, 0, data_list[i]->vertices.size(), 15); //glDrawArrays(data_list[i]->drawType,0, data_list[i]->vertices.size());
		glBindVertexArray(0);
	}
}

void Render::polygonGenerator(unsigned int n, std::string name, float size, std::vector<GLfloat> colours) {
	//Since we do not use textures here, we have a seperate v/f shader. 
	//Please use copy function for creating a copy instead of creating a new object. TODO implement instancing!.

	if (colours.size() != 4) {
		std::cerr << "size of colour parameter must be 4...setting default colour parameter (white)";
		colours = std::vector<GLfloat>{ 1.0f, 1.0f, 1.0f, 1.0f };
	}

	objectData* polygon = new objectData; //Preallocate data for the object
	polygon->ID=count;
	polygon->shader = shaderinstance;
	polygon->drawType = GL_TRIANGLES;
	polygon->Attribs.VAO = generateVAO();
	bindVertexObject(polygon->Attribs.VAO);
	polygon->Attribs.VBO = generateVBO();
	polygon->Attribs.TAO = emptyTex;
	//Begin vertices
	std::vector<GLfloat> data;
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	GLfloat z = 0.0f;

	polygon->vertices.reserve(n);

	if (n > 4) { //if polygon has more vertices than square, we need a center point
		data.reserve(7 * (n) + 7);
		data.push_back(x);
		data.push_back(y);
		data.push_back(z);
		//tex coordinates
		data.push_back(0);
		data.push_back(0);
		for (int j = 0; j < 4; j++)
			data.push_back(colours[j]);

		polygon->vertices.reserve(n+1);
		polygon->vertices.push_back(glm::vec3(0.0f,0.0f,0.0f));
	}
	else 
		data.reserve(7*n);

	for (unsigned int i = 0; i < n; i++) {
		x = size * (cos((float)i * (2 * PI / (float)(n))) * cos(PI / (float)(n)) - sin((float)i * (2 * PI / (float)(n))) * sin(PI / (float)(n)));
		y = size * (sin((float)i * (2 * PI / (float)(n))) * cos(PI / (float)(n)) + cos((float)i * (2 * PI / (float)(n))) * sin(PI / (float)(n)));

		polygon->vertices.push_back(glm::vec3(x,y,z));

		data.push_back(x);
		data.push_back(y);
		data.push_back(z);
		//texture coordinates
		data.push_back(0);
		data.push_back(0);
		for (int j = 0; j < 4; j++)
			data.push_back(colours[j]);
	}
	//end vertices
	bindBuffers(polygon->Attribs.VBO, data);
	polygon->Attribs.EBO = generateEBO();

	//begin indices
	std::vector<GLshort> data_i;
	if (n == 3) {
		data_i.push_back(0);
		data_i.push_back(1);
		data_i.push_back(2);
	}
	else if (n == 4) {
		data_i.push_back(0);
		data_i.push_back(1);
		data_i.push_back(2);
		data_i.push_back(2);
		data_i.push_back(3);
		data_i.push_back(0);
	}
	else {
		data_i.push_back(0); //end triangle
		data_i.push_back(n);
		data_i.push_back(1);
		for (int i = 0; i < n; i++){ //loop creating triangles
			data_i.push_back(0);
			data_i.push_back(1+i);
			data_i.push_back(2+i);
		}
		
	}
	//end indices
	polygon->indices.swap(data_i);

	bindIndices(polygon->Attribs.EBO, polygon->indices); //CREATE TWO COPIES 1 for VAO, positions etc, other for myObjData type!

	//Position/Colours attributes!
	vertexAttribute(0, 3, ptc, 0); //position
	vertexAttribute(1, 4, ptc, 5); //colour
	vertexAttribute(2, 2, ptc, 3); //tex

	bindVertexObject(); // unbind vao's/vbo's
	pub_objects[name.c_str()] = *polygon;

	data_list.push_back(polygon);
	count += 1;
}

/*std::vector<GLfloat> Render::PixelColours(objectData* object_data, int n, GLfloat* colours) { //n is number of vertex
	std::vector<GLfloat> colour_vector;
	colour_vector.reserve(4 * n);
	for (size_t i = 0; i < n; i++) {
		colour_vector.push_back(colours[0]);
		colour_vector.push_back(colours[1]);
		colour_vector.push_back(colours[2]);
		colour_vector.push_back(colours[3]);
	}
	return colour_vector;
}
*/


// NEEEDS TO BE INTERGRATED WITH DATATYPE 
void Render::initTextRender(const char* font, int size) {
	if (FT_Init_FreeType(&this->ft))
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	if (FT_New_Face(this->ft, "fonts/arial.ttf", 0, &this->face))
		std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Set_Pixel_Sizes(this->face, 0, size);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; c++) {
		// load character glyph
		if (FT_Load_Char(this->face, c, FT_LOAD_RENDER)) {
			std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		//bindTexture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
		GLuint TAO = 0;
		Character character = {
			TAO,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Render::RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 colour) {
	s.use();
	GLfloat colour_f[] = { colour.x, colour.y, colour.z };
	s.setFloatNd("textColour", 3, colour_f);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////